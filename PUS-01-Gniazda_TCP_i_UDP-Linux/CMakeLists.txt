cmake_minimum_required(VERSION 3.6)
project(PUS_01_Gniazda_TCP_i_UDP_Linux)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/server5.c)

add_executable(PUS_01_Gniazda_TCP_i_UDP_Linux ${SOURCE_FILES})