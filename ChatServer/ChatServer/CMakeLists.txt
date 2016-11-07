cmake_minimum_required(VERSION 3.3)
project(Chat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES
    DieWithError.c
    HandleTCPClient.c
    Server.c
    structs.h)

add_executable(Chat ${SOURCE_FILES})