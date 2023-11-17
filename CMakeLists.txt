cmake_minimum_required(VERSION 3.15)
project(nanonet)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(nanonet_static STATIC ${SRC_LIST})
add_library(nanonet SHARED ${SRC_LIST})

