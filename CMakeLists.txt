cmake_minimum_required(VERSION 2.8)

project(link_manager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

add_executable(manager src/manager.cpp)
target_link_libraries(manager pthread)

add_executable(slaver src/slaver.cpp)
target_link_libraries(manager pthread)
