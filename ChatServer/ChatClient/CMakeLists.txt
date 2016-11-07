cmake_minimum_required(VERSION 3.3)
project(ChatClient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES
    Client.c
    DieWithError.c)

add_executable(ChatClient ${SOURCE_FILES})