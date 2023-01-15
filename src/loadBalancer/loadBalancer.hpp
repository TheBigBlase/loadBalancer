#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP
#define BOOST_BIND_GLOBAL_PLACEHOLDERS//bad practice, but removes a warning
#include "opt.hpp"
#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class Machine { //represent a machine(server) from the standpoint of a load balancer
								//we only have to do here the "client" interface "talking" to them
	public:
		Machine(std::string addr, std::string port, boost::asio::io_context & io);


		void connect();

		void request(std::string path, boost::asio::streambuf *);

		std::string getPort() {
			return this->port;
		}

		std::string getAddr() {
			return this->addr;
		}

	private:
		void read();
		std::string port;
		std::string addr;
		void readHandler(const boost::system::error_code& error, 
				std::size_t bytes_transferred);
		void writeHandler(const boost::system::error_code& error, 
				std::size_t bytes_transferred);

		boost::asio::io_context &io_context_;
		tcp::resolver resolver_;
		tcp::socket socket_;
		boost::asio::streambuf *buffer_;
};

class BalancerConnection
  : public boost::enable_shared_from_this<BalancerConnection> {
	public:
		typedef boost::shared_ptr<BalancerConnection> pointer;

		static pointer create(boost::asio::io_context& io_context) {
			return pointer(new BalancerConnection(io_context));
		}

		tcp::socket& socket()	{
			return socket_;
		}

		void start(Machine * m){
			this->machine_ = m;
			read();//just read, other methods are in "callbacks" (handlers)
		}

		//remains of the async dream
		void handle_read(const boost::system::error_code&, size_t bytes_transferred);

	private:
		BalancerConnection(boost::asio::io_context& io_context)
				: socket_(io_context)	{
			buffer_.prepare(1024);
		}

		void read();

		void sendToClient();
		void sendToServer();

		void handleWrite(const boost::system::error_code&, size_t bytes_transferred);

		Machine * machine_;
		tcp::socket socket_;
		std::string message_;
		boost::asio::streambuf buffer_;
};


class BalancerServer {
	public:
		BalancerServer(boost::asio::io_context& ioContext, 
				const int serverPort, std::vector<Machine *>& vecMachines)
				: ioContext_(ioContext),
				acceptor_(ioContext, tcp::endpoint(tcp::v4(), serverPort)){
			startAccept();
			this->vectorMachine_ = &vecMachines;
		}

		void run();

	private:
		void startAccept();
		void handleAccept(BalancerConnection::pointer new_connection,
				const boost::system::error_code& error);
		Machine * selectMachine(){
			return (*vectorMachine_)[last_index_used_++ % vectorMachine_->size()];
			//wtf ? vec.size return a float ???
		}

		boost::asio::io_context& ioContext_;
		tcp::acceptor acceptor_;
		int last_index_used_ = -1;
		std::vector<Machine *> * vectorMachine_;
};

class LoadBalancer{
	public:
		LoadBalancer(boost::asio::io_context&, int port);
		void run();
		void connectTo(Machine *);//useless now ?
		//void connectAll();
		BalancerServer * startServer(boost::asio::io_context&);
		Machine * initMachine(std::string addr, std::string port);
		void sendToAll(std::string);
		

	private: 
		std::vector<Machine * > machines_;
		BalancerServer * server_;
		const int port_;

		boost::asio::io_context& ioContext_;
};


#endif
