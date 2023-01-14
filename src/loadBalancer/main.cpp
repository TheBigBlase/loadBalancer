//loadBalancer
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "loadBalancer.hpp"

int main(int argc, char* argv[]){
  try{
    if (argc != 1){
      std::cerr << "Usage: loadBalancer (the machines are defined in the opt.hpp file)" << std::endl;
      return 1;
    }

		boost::asio::io_context io_context;
		LoadBalancer loadBalancer(io_context, 6000);
		std::cout << "loaded" << std::endl;
		loadBalancer.sendToAll("no\r\n");
		loadBalancer.run();
	}
  catch (std::exception& e)
  {
		std::cout << "h" << std::endl;
    std::cerr << e.what() << std::endl;
  }

 
  return 0;
}
