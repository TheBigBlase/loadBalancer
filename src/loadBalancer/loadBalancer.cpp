#include "loadBalancer.hpp"
#include "../shared/tcp.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

void BalancerServer::run() {
	this->io_context_.run();
}

void Machine::connect() {
	tcp::resolver::results_type endpoints 
		= this->resolver_.resolve(this->addr, this->port);
	boost::asio::connect(this->socket_, endpoints);
}

Machine::Machine(std::string addr, std::string port)
		: socket_(io_context_), resolver_(io_context_), io_context_() {
	;
	
	this->port = port;
	this->addr = addr;
	std::cout << this << std::endl;
}


void Machine::request(std::string req) {
	std::cout << "[MACHINE] sending " << req << "to :" 
		<< this->addr << ":" << this->port << std::endl;
	this->socket_.send(boost::asio::buffer(req));
}

void LoadBalancer::connectTo(Machine *m) {
	m->connect();
}

void LoadBalancer::connectAll() {
	for(auto * m : this->machines) {
		m->connect();
	}
}

BalancerServer * LoadBalancer::startServer(boost::asio::io_context& serverContext) {
	this->server = new BalancerServer(serverContext, this->port);
	return this->server;
}

Machine * LoadBalancer::initMachine(std::string address, std::string port) {
	auto * machine = new Machine(address, port);
	machine->connect();
	return machine;
}

LoadBalancer::LoadBalancer(int port) {
	boost::asio::io_service serverContext;
	//idk why i have to do that like that, but if i declare the server io context
	//inside his class, a segfault occurs
	this->startServer(serverContext);
	//thread didnt work ? or i didnt understand it at all
	//anyway, launch it asyncly

	boost::thread t{boost::bind(&boost::asio::io_service::run, &serverContext)};
	std::cout << serverContext.stopped() << std::endl;
	sleep(1);

	for(auto k : machinesOpt) {//for all that are declared in the option header
		auto tmp = new Machine(k.first, k.second);
		tmp->connect();
		std::cout << "[MACHINE] connected to " << k.first << ":" << k.second <<std::endl;
	}
	this->port = port;
}

void LoadBalancer::sendToAll(std::string req) {
	for(Machine * m : this->machines) {
		m->request(req);
	}
}
