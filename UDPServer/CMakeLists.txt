cmake_minimum_required(VERSION 3.10)

project(UDPServer)

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 20)

include_directories(../Common)

set (SRC ServerConfig.cpp
         ../Common/Networking.cpp
         main.cpp)

add_executable(UDPServer ${SRC})

if(WIN32)
  target_link_libraries(UDPServer wsock32 ws2_32)
endif()