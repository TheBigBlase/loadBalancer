#include "loadBalancer.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

Machine * LoadBalancer::initMachine(std::string address, std::string port){
	auto * machine = new Machine(address, port);
	machine->connect();
	return machine;
}

LoadBalancer::LoadBalancer(){
	for(auto k : machinesOpt){
		std::cout << k.first << k.second;
		auto tmp = new Machine(k.first, k.second);
		tmp->connect();
		this->machines.push_back(tmp);
	}
}

void Machine::run(){
}

void Machine::connect(){
	tcp::resolver::results_type endpoints = resolver_.resolve(this->addr, this->port);
	boost::asio::connect(this->socket_, endpoints);
}

Machine::Machine(std::string addr, std::string port) :
	resolver_(tcp::resolver(io_context_)),
	socket_(io_context_)
{
	this->port = port;
}

void Machine::request(std::string path){

};
