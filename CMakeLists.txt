cmake_minimum_required(VERSION 3.25)
project(loadBalancer)

set(CMAKE_CXX_STANDARD 17)

set(useThisAsAComment this doesnt need to be here, it's not using thread anymore)
find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(client
        src/client/main.cpp
        )

add_executable(loadBalancer
        src/loadBalancer/loadBalancer.cpp
        src/loadBalancer/main.cpp
        )

add_executable(server
        src/server/main.cpp
        )

target_link_libraries(loadBalancer ${Boost_LIBRARIES})
