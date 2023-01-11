#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP
#include "opt.hpp"
#include <vector>
#include <thread>
#include "../shared/tcp.hpp"
#include <boost/shared_ptr.hpp>

class balancerConnection : public tcp_connection<balancerConnection> {
	public:

		void handle_write(const boost::system::error_code&, size_t bytes_transferred)
		{
			//then : 
			boost::asio::async_read_until(socket_, buffer_, "\r\n",
					boost::bind(&balancerConnection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

		void start(){
			//write request. Does not need to be async ?
			boost::asio::async_write(socket_, boost::asio::buffer(request_),
					boost::bind(&balancerConnection::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

		void handle_read(const boost::system::error_code&, size_t bytes_transferred){
			message_ = "hello " + message_ + " im balancer";

			message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), 
						std::istreambuf_iterator<char>() );
			std::cout << "[BALANCER]: " << message_ << std::endl;
		}
};




class Machine { //represent a machine(server) from the standpoint of a load balancer
	public:

		Machine(std::string addr, std::string port);

		void connect();

		void run();//yes
		void request(std::string path);


	//	tcp::socket socket(io_context);
	//	boost::asio::connect(socket, endpoints);

	//	socket.send(boost::asio::buffer("client\r\n"));

//		std::cout << buf.data() << std::endl ;
	//	}
	//
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
		LoadBalancer(int port);
		void connectTo(Machine *);
		void connectAll();
		void startServer();
		Machine * initMachine(std::string addr, std::string port);
		

	private: 
		std::vector<Machine * > machines;
		tcp_server<balancerConnection> * server;
		int port;
};

#endif
