#include <iostream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "server.hpp"

using boost::asio::ip::tcp;

int main(int argc, char** argv){
  try
  {
    boost::asio::io_context io_context;
    Tcp_server server(io_context, 6001);
    io_context.run();	
	}
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
