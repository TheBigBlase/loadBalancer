//client
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]){
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <port>" << std::endl;
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], argv[2]);

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

		socket.send(boost::asio::buffer("client\r\n"));

    for (;;)
    {
      boost::array<char, 128> buf;
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(buf), error);

      if (error == boost::asio::error::eof){
				std::cout << "server closed connection" << std::endl;
        break; // Connection closed cleanly by peer.
			}
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      std::cout << buf.data() << std::endl ;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
