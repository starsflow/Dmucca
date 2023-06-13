cmake_minimum_required(VERSION 3.12)
project(Dmucca)
set(CMAKE_CXX_STANDARD 17)
set(JEMALLOC external/jemalloc)
set(GTEST external/googletest)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -L${JEMALLOC}/lib -Wl,-rpath,${JEMALLOC}/lib -Wall -Wno-long-long -Wno-unused-variable -Wno-variadic-macros -pedantic -O2")
include_directories(${JEMALLOC}/include/ ${GTEST}/include/ benchmark/ycsb/ common/ system/ utils/ protocol/ ./) 
link_libraries(${JEMALLOC})

if(APPLE)
    find_package(glog REQUIRED)
    
    find_package(gflags REQUIRED)
    link_libraries(glog::glog gflags)
else()
    link_libraries(glog gflags)
endif()

add_executable(Dmucca test.cpp)