#include "loadBalancer.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

void BalancerServer::run(){
	this->ioContext_.run();
}

void Machine::connect() {
	tcp::resolver::results_type endpoints 
		= this->resolver_.resolve(this->addr, this->port);
	boost::asio::connect(this->socket_, endpoints);
}

Machine::Machine(std::string addr, std::string port, boost::asio::io_context &io)
		: socket_(io_context_), resolver_(io_context_), 
		io_context_(io) {
	this->port = port;
	this->addr = addr;
}

void Machine::writeHandler(const boost::system::error_code& error, 
		std::size_t bytes_transferred) {
	//std::cerr << error << std::endl;
	boost::asio::read(socket_, *this->buffer_);
}

void Machine::readHandler(const boost::system::error_code& error, 
		std::size_t bytes_transferred) {
	std::cout << "[MACHINE] response from machine : " << this->buffer_ << std::endl;
}

void Machine::request(std::string req, boost::asio::streambuf *buf) {
	//WHY DOES A SOCKET CLOSE WHEN FINISHED SENDING EVERYTHING
	//took me 2 days at least to find this out
	this->connect();//aha funny function :)
	buf->prepare(1024);
	std::cout << "[MACHINE] sending " << req << "to :"
		<< this->addr << ":" << this->port << std::endl;
	//this->socket_.write_some(boost::asio::buffer(req));
	boost::asio::write(socket_, boost::asio::buffer(req));
	//i cant make async work for this part, switching to sycn
	//i spent too much time on random issues, i cant be bothered
	boost::system::error_code error;
	try {
		boost::asio::read_until(socket_, *buf, "\r\n", error);
	}
	catch (std::exception e){
		std::cout << "here" << std::endl;
		std::cerr << e.what();
	}
}

void LoadBalancer::connectTo(Machine *m) {
	m->connect();
}

BalancerServer * LoadBalancer::startServer(boost::asio::io_context& serverContext) {
	this->server_ = new BalancerServer(serverContext, this->port_, this->machines_);
	return this->server_;
}

Machine * LoadBalancer::initMachine(std::string address, std::string port) {
	auto * machine = new Machine(address, port, this->ioContext_);
	machine->connect();
	return machine;
}

LoadBalancer::LoadBalancer(boost::asio::io_context& io_context, const int port) :port_{port}, ioContext_(io_context){

	for(auto k : machinesOpt) {//for all that are declared in the option header
		auto tmp = new Machine(k.first, k.second, ioContext_);
		tmp->connect();
		std::cout << "[MACHINE] connected to " << k.first << ":" << k.second <<std::endl;
		machines_.push_back(tmp);
	}

	this->startServer(ioContext_);
}

void LoadBalancer::sendToAll(std::string req) {
	for(Machine * m : this->machines_) {
		boost::asio::streambuf buf;
		std::cout << m->getAddr() << std::endl;
		m->request(req, &buf);
	}
}

void LoadBalancer::run() {
	this->ioContext_.run();
}


void BalancerConnection::handle_read(const boost::system::error_code&, size_t bytes_transferred) {
	//runs once socket has finished

	message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), 
				std::istreambuf_iterator<char>() );

	std::cout << "[BALANCER]: new message: \"" << message_ << "\"" << std::endl;
	sendToServer();
}
	void BalancerConnection::read(){
	boost::asio::async_read_until(socket_, buffer_, "\r\n",
			boost::bind(&BalancerConnection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void BalancerConnection::sendToServer() {
	this->machine_->request(message_, &buffer_);
	sendToClient();
}

void BalancerConnection::sendToClient(){
	boost::asio::async_write(socket_, buffer_,
			boost::bind(&BalancerConnection::handleWrite, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}
void BalancerConnection::handleWrite(const boost::system::error_code&, size_t bytes_transferred) {
	message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), 
				std::istreambuf_iterator<char>() );
	std::cout << message_ << std::endl;
	std::cout << "[MACHINE] ended connection, sending to client" << std::endl;
}

void BalancerServer::startAccept() {
	BalancerConnection::pointer new_connection =
		BalancerConnection::create(ioContext_);

	acceptor_.async_accept(new_connection->socket(),
			boost::bind(&BalancerServer::handleAccept, this, new_connection,
				boost::asio::placeholders::error));
}

void BalancerServer::handleAccept(BalancerConnection::pointer new_connection,
		const boost::system::error_code& error) {
	if (!error) {
		std::cout << "[SERVER] new con" << std::endl;
		new_connection->start(selectMachine());
	}
	startAccept();
}



