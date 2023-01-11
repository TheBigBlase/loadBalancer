//loadBalancer
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "loadBalancer.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]){
  try{
    if (argc != 1){
      std::cerr << "Usage: loadBalancer (the machines are defined in the opt.hpp file)" << std::endl;
      return 1;
    }

	boost::asio::io_context io_context;
	LoadBalancer loadBalancer(6000);
	}

  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

 
  return 0;
}
