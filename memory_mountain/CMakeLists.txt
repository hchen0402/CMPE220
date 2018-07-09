cmake_minimum_required(VERSION 3.10)
project(memory_mountain C)

set(CMAKE_C_STANDARD 99)

add_executable(memory_mountain main.c clock.c clock.h fcyc2.c fcyc2.h)