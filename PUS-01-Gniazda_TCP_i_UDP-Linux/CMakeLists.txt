cmake_minimum_required(VERSION 3.6)
project(PUS_01_Gniazda_TCP_i_UDP_Linux)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    bin/img/mars-top.gif
    bin/img/mars2.jpg
    bin/img/pklogo.gif
    src/client1.c
    src/client2.c
    src/libpalindrome.c
    src/libpalindrome.h
    src/server1.c
    src/server2.c
    Makefile
    PUS-01-Gniazda_TCP_i_UDP-Linux.pdf src/server3.c)

add_executable(PUS_01_Gniazda_TCP_i_UDP_Linux ${SOURCE_FILES})