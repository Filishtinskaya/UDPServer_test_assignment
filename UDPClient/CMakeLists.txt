cmake_minimum_required(VERSION 3.10)

project(UDPClient)

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 20)

include_directories(../Common)

add_executable(UDPClient ClientConfig.cpp main.cpp)

if(WIN32)
  target_link_libraries(UDPClient wsock32 ws2_32)
endif()