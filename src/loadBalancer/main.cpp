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

		LoadBalancer loadBalancer(6000);
		std::cout << "loaded" << std::endl;
	}
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

 
  return 0;
}
