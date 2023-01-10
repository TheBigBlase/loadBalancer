cmake_minimum_required(VERSION 3.25)
project(loadBalancer)

set(CMAKE_CXX_STANDARD 17)

add_executable(client
        src/client/client.cpp
        src/client/main.cpp
        )

add_executable(loadBalancer
        src/loadBalancer/loadBalancer.cpp
        src/loadBalancer/main.cpp
        )

add_executable(server
        src/server/server.cpp
        src/server/main.cpp
        )
