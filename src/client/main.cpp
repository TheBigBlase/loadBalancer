//client
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]){
  try {
    if (argc != 4) {
      std::cerr << "Usage: client <host> <port> <request>" << std::endl;
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], argv[2]);

		//check if ends with \r\n, if not adds it
		if(argv[3][0] == '\0'){//if string is empty, print error
      std::cerr << "Usage: client <host> <port> <request>" << std::endl;
      return 1;
		}

		char l = argv[3][0];
		char k = argv[3][1];//not unsafe, since there is a char * len >= 1
		for(char m {0} ; argv[3][m] != '\0' ; m++){
			k = argv[3][m];
			l = k;
		}

		std::string toSend;

		if(!(k == '\n' && l == '\r')) {//adds if not in it
			std::stringstream ss;
			ss << argv[3] << "\r\n";
			toSend = ss.str();
		}
		//im sure theres a better way to see it but hey

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

		socket.send(boost::asio::buffer(toSend));

    for (;;) {
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
