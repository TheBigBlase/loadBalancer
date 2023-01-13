#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>

using boost::asio::ip::tcp;

class Tcp_connection
  : public boost::enable_shared_from_this<Tcp_connection>{
	public:
		typedef boost::shared_ptr<Tcp_connection> pointer;

		static pointer create(boost::asio::io_context& io_context)
		{

			return pointer(new Tcp_connection(io_context));
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		void read_file(){

		}

		void write_file(){

		}

		void start(){
			//read until then write
			boost::asio::async_read_until(socket_, buffer, "\r\n",
					boost::bind(&Tcp_connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}

	private:
		Tcp_connection(boost::asio::io_context& io_context)
			: socket_(io_context)
		{
			buffer.prepare(1024);
		}

		void handle_read(const boost::system::error_code& /*error*/,
				size_t bytes_transferred){
			message_ = std::string( (std::istreambuf_iterator<char>(&buffer)), std::istreambuf_iterator<char>() );
			std::cout << "read: " << message_ << std::endl;
			//then : 
			//socket_.async_write_some(boost::asio::buffer(message_),
			//		boost::bind(&Tcp_connection::handle_write, shared_from_this(),
			//			boost::asio::placeholders::error,
			//			boost::asio::placeholders::bytes_transferred));
		}

		void handle_write(const boost::system::error_code& /*error*/,
				size_t bytes_transferred){
			message_ = "hello " + message_ + " im server";
			std::cout << "send: " << message_ << std::endl;
		}

		tcp::socket socket_;
		std::string message_;
		boost::asio::streambuf buffer;
};


class Tcp_server{
	public:
		Tcp_server(boost::asio::io_context& io_context, int port)
			: io_context_(io_context),
				acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

	private:
		void start_accept()
		{
			Tcp_connection::pointer new_connection =
				Tcp_connection::create(io_context_);

			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&Tcp_server::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

		void handle_accept(Tcp_connection::pointer new_connection,
				const boost::system::error_code& error)
		{
			if (!error)
			{
				new_connection->start();
			}

			start_accept();
		}

		boost::asio::io_context& io_context_;
		tcp::acceptor acceptor_;
};

