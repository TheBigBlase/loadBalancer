#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP
#include "opt.hpp"
#include <vector>
#include <thread>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>

using boost::asio::ip::tcp;

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

		void start(){
			read();//just read, other methods are in "callbacks" (handlers)
		}

	private:
		BalancerConnection(boost::asio::io_context& io_context)
				: socket_(io_context)	{
			buffer_.prepare(1024);
		}

		void read(){
			boost::asio::async_read_until(socket_, buffer_, "\r\n",
					boost::bind(&BalancerConnection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}

		void write_handler(const boost::system::error_code&, size_t bytes_transferred) {
			read();
		}

		void handle_read(const boost::system::error_code&, size_t bytes_transferred) {
			//runs once socket has finished

			message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), 
						std::istreambuf_iterator<char>() );
			std::cout << "[BALANCER]: " << message_ << std::endl;
			write();
		}

		void write() {
			boost::asio::async_write(socket_, buffer_,
				boost::bind(&BalancerConnection::write_handler, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		tcp::socket socket_;
		std::string message_;
		boost::asio::streambuf buffer_;
};


class BalancerServer {
	public:
		BalancerServer(boost::asio::io_context& io_context, const int serverPort)
				: io_context_(io_context),
				acceptor_(io_context, tcp::endpoint(tcp::v4(), serverPort)){
			start_accept();
		}

		void run();

	private:
		void start_accept() {
			BalancerConnection::pointer new_connection =
				BalancerConnection::create(io_context_);

			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&BalancerServer::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

		void handle_accept(BalancerConnection::pointer new_connection,
				const boost::system::error_code& error) {
			if (!error) {
				std::cout << "[SERVER] new con" << std::endl;
				new_connection->start();
			}

			start_accept();
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;
};



class Machine { //represent a machine(server) from the standpoint of a load balancer
								//we only have to do here the "client" interface "talking" to them
	public:
		Machine(std::string addr, std::string port);

		void connect();

		void request(std::string path);


		std::string getPort(){
			return this->port;
		}

		std::string getAddr(){
			return this->addr;
		}

	private:
		std::string port;
		std::string addr;

		boost::asio::io_context io_context_;
		tcp::resolver resolver_;
		tcp::socket socket_;
};

class LoadBalancer{
	public:
		LoadBalancer(boost::asio::io_context&, int port);
		void run();
		void connectTo(Machine *);
		void connectAll();
		BalancerServer * startServer(boost::asio::io_context&);
		Machine * initMachine(std::string addr, std::string port);
		void sendToAll(std::string);
		

	private: 
		std::vector<Machine * > machines_;
		BalancerServer * server_;
		const int port_;
		boost::asio::io_context& io_context_;
};


#endif
