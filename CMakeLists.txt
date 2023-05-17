cmake_minimum_required(VERSION 3.12)
project(Dmvcca)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wno-long-long -Wno-unused-variable -Wno-variadic-macros -pedantic -O2")
include_directories(benchmark/ycsb/ storage/ system/ utils/ protocol/)

add_executable(Dmvcca test.cpp)