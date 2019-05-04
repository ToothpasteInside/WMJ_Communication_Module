cmake_minimum_required(VERSION 2.8)
project(CAN_CommunicationModule)
add_compile_options(--std=c++11)


include_directories(include)

add_library(utils src/utils.cpp)
add_library(TransportCan src/TransportCan.cpp)
target_link_libraries(TransportCan utils pthread)
