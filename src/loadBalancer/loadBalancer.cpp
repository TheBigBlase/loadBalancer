#include "loadBalancer.hpp"
#include "../shared/tcp.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

Machine * LoadBalancer::initMachine(std::string address, std::string port){
	auto * machine = new Machine(address, port);
	machine->connect();
	return machine;
}

LoadBalancer::LoadBalancer(int port){
	for(auto k : machinesOpt){
		std::cout << k.first << k.second;
		auto tmp = new Machine(k.first, k.second);
		tmp->connect();
		this->machines.push_back(tmp);
	}
	this->port = port;
}

void Machine::run(){
}

void Machine::connect(){
	tcp::resolver::results_type endpoints = this->resolver_.resolve(this->addr, this->port);

	boost::asio::connect(this->socket_, endpoints);
}

Machine::Machine(std::string addr, std::string port) :
	resolver_(tcp::resolver(io_context_)),
	socket_(io_context_)
{
	this->port = port;
	this->addr = addr;
}

void Machine::request(std::string path){

}

void LoadBalancer::connectTo(Machine *m){
	m->connect();
}

void LoadBalancer::connectAll(){
	for(auto * m : this->machines){
		m->connect();
	}
}

void LoadBalancer::startServer(){
	boost::asio::io_context io_context;
	this->server = new tcp_server<balancerConnection>(io_context, this->port);
}
