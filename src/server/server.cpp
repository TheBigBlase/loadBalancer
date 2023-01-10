#include "server.hpp"

namespace server{
	using boost::asio::ip::tcp;
	std::string handleConnection(tcp::socket * socket, std::string message){
		message.resize(1024);
		boost::system::error_code ignored_error;

		boost::asio::read(*socket, boost::asio::buffer(message), ignored_error);

		std::cout << "message: " << message << std::endl;

		boost::asio::write(*socket, boost::asio::buffer(message), ignored_error);
		return message;
	}
}
