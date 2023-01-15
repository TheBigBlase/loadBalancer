# loadBalancer

This is a student project to do a low level, network oriented load balancer.
It is written in c++ and uses boost.
It structure itself around the notion of client (the one requesting a file),
server (the one hosting the file) and load balancer (the one redirecting the client to a server).
It is here assumed that all server are connected to a single hard drive hosting everything (handy 
for testing :) )


## installation
```sh
mkdir build && cd build && cmake ../ && make
```

## usage
first define your servers in `src/loadBalancer/opt.hpp`.
Then launch a client with `./client <loadBalancerHostName> <loadBalancerPort> <get [filename] | write [filename] [content] | echo [msg]>` 
after having build the project. 

## what works & doesnt
everything, but i couldnt manage to do a part in a async way, rendering the whole structure synchronous,
therefore useless. Also does not provide safety for writting : you can still read and write at the same time.

## reflections
boost::asio was way too hard for me to apprehend within the given time. Some basic issue took me way too much time 
to fix, or to even spot. I looked for an eternity in the wrong direction before realising the answer was really stupid.
Some of theses issue could have been really easily spotted if pair programming.  
Furthermore, i first tried to implement this project with abstract classes, and there was a smart pointer "ownership" issue
giving me headaches, so this is why this branch exists.
Also, some of this code comes from SO, or tutorials, and switch styles in between, so it looks REALLY messy.

thx for your attention, gl for grading, this is some of the most nonsensical work i have ever done
