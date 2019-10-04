cmake_minimum_required(VERSION 3.15)
project(exp1 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


add_executable(exp1 main.c)