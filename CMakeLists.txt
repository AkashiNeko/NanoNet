cmake_minimum_required(VERSION 3.15)
project(nanonet)

set(CMAKE_CXX_STANDARD 11)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(nanonet_static STATIC ${SRC_LIST}
        src/socket_base.cpp
        src/socket_base.h)
add_library(nanonet SHARED ${SRC_LIST}
        src/socket_base.cpp
        src/socket_base.h)

set_target_properties(nanonet PROPERTIES OUTPUT_NAME "nanonet")
set_target_properties(nanonet_static PROPERTIES OUTPUT_NAME "nanonet")

