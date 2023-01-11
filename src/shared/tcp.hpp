#ifndef TCP_HPP
#define TCP_HPP
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>

using boost::asio::ip::tcp;

template <class Argument>
class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection<Argument>>{
	public:
		typedef boost::shared_ptr<tcp_connection<Argument>> pointer;

		void setRequest(std::string req)
		{
			this->mRequest = req;
		}

		static pointer create(boost::asio::io_context& io_context)
		{
			return pointer(new tcp_connection<Argument>(io_context));
		}

		tcp::socket& socket()
		{
			return socket_;
		}

		virtual void start(){
			//read until then write
			boost::asio::async_read_until(socket_, buffer_, "\r\n",
					boost::bind(&Argument::handle_read, boost::static_pointer_cast<Argument>(this->shared_from_this()),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

	protected:
		std::string request_;

		tcp_connection<Argument>(boost::asio::io_context& io_context)
			: socket_(io_context)
		{
			buffer_.prepare(1024);
		}

		virtual void handle_read(const boost::system::error_code& /*error*/,
				size_t bytes_transferred){
			message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), std::istreambuf_iterator<char>() );
			std::cout << "read: " << message_ << std::endl;
			//then : 
			socket_.async_write_some(boost::asio::buffer(message_),
					boost::bind(&Argument::handle_write, boost::static_pointer_cast<Argument>(this->shared_from_this()),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

		virtual void handle_write(const boost::system::error_code& /*error*/,
				size_t bytes_transferred){
			message_ = "hello " + message_ + " im server";
			std::cout << "send: " << message_ << std::endl;
		}

		tcp::socket socket_;
		std::string message_;
		boost::asio::streambuf buffer_;
};


template <typename Argument>
class tcp_server{
	public:
		tcp_server<Argument>(boost::asio::io_context& io_context, int port)
			: io_context_(io_context),
				acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

	private:
		void start_accept(){
			typename tcp_connection<Argument>::pointer new_connection = tcp_connection<Argument>::create(io_context_);

			acceptor_.async_accept(new_connection->socket(),
					boost::bind(&tcp_server<Argument>::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

		void handle_accept(typename tcp_connection<Argument>::pointer new_connection,
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


#endif
