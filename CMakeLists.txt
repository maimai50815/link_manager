cmake_minimum_required(VERSION 2.8)

project(link_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(main1 main1.cpp)
add_executable(main2 main2.cpp)
