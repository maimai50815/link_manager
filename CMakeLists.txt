cmake_minimum_required(VERSION 2.8)

project(link_manager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

add_executable(link_master src/link_master.cpp)
target_link_libraries(link_master pthread)

add_executable(manager test/manager.cpp)
target_link_libraries(manager pthread)

add_executable(slaver test/slaver.cpp)
target_link_libraries(slaver pthread)

add_executable(monitor test/monitor.cpp)
target_link_libraries(monitor pthread)
