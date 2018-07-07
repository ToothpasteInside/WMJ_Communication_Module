cmake_minimum_required(VERSION 2.8)
project(CAN_CommunicationModule)
add_compile_options(--std=c++11)


include_directories(include)

add_library(utils src/utils.cpp)
add_library(transport_can src/transport_can.cpp)
target_link_libraries(transport_can utils pthread)
