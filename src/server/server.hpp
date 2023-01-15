#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>

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
		void start(){
			//read until then write
			boost::asio::async_read_until(socket_, buffer_, "\r\n",
					boost::bind(&Tcp_connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

	private:
		Tcp_connection(boost::asio::io_context& io_context)
			: socket_(io_context)
		{
			buffer_.prepare(1024);
		}

		void handle_read(const boost::system::error_code& /*error*/,
				size_t bytes_transferred){
			message_ = std::string( (std::istreambuf_iterator<char>(&buffer_)), std::istreambuf_iterator<char>() );
			std::cout << "[SERVER] recived: \"" << message_ << "\"" << std::endl;
			//then : 
			handleCommands(message_);
		}

		void write(std::string msg){
			socket_.async_send(boost::asio::buffer(msg),
					boost::bind(&Tcp_connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
		}

		void handle_write(const boost::system::error_code&,
				size_t bytes_transferred){
			std::cout << "[SERVER] connection closed." << std::endl;
			//lying, it is close at the next operation, but close enough
		}


		std::vector<std::string> tokenizeWithSpace(std::string input){
			auto const regex = std::regex{R"(\s+)"};//one or more space
			auto const vec = std::vector<std::string>(
					std::sregex_token_iterator{begin(input), end(input), regex, -1},
					std::sregex_token_iterator{}
			);
			return vec;
		}

		std::string handleCommands(std::string request){
			auto commandAndArgs = tokenizeWithSpace(request);
			
			if(commandAndArgs[0] == "get"){//compare first 3 chars
				std::cout << "[GET] reading " << commandAndArgs[1] <<std::endl;
				//commandAndArgs.erase(std::find(commandAndArgs.begin(), commandAndArgs.end(), 0));
				this->read_file(std::ifstream{commandAndArgs[1]});
			}

			else if(commandAndArgs[0] == "write"){//compare first 3 chars
				std::cout << "[WRITE] writing" << commandAndArgs[1] <<std::endl;
				write_file(std::ofstream{commandAndArgs[1]}, commandAndArgs[2]);
			}

			else if(commandAndArgs[0] == "echo"){//compare first 3 chars
				std::cout << "[ECHO] " << commandAndArgs[1] <<std::endl;
				write(request);
			}

			else{
				std::cout << "[COMMANDS] did not recognize command" << std::endl;
			}
			return "";
		}

		void read_file(std::ifstream file){
			std::string buffer;
			while (getline(file, buffer)) {
				// Output the text from the file
				write(buffer);
			}
			//write("\r\n");
		}

		void write_file(std::ofstream file, std::string content){
			file << content;
		}

		tcp::socket socket_;
		std::string message_;
		boost::asio::streambuf buffer_;
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

