cmake_minimum_required(VERSION 3.15)
project(nanonet)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(nanonet SHARED ${SRC_LIST})
add_library(nanonet_static STATIC ${SRC_LIST})

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_link_libraries(nanonet PRIVATE "-lws2_32")
target_link_libraries(nanonet_static PRIVATE "-lws2_32")
endif()

set_target_properties(nanonet PROPERTIES OUTPUT_NAME "nanonet")
set_target_properties(nanonet_static PROPERTIES OUTPUT_NAME "nanonet")
