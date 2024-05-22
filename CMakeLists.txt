cmake_minimum_required(VERSION 3.15)
project(nanonet)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(nanonet SHARED ${SRC_LIST})
add_library(nanonet_static STATIC ${SRC_LIST})

install(TARGETS nanonet
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(TARGETS nanonet_static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
endif()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/nanonet.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(nanonet PRIVATE "-lws2_32")
    target_link_libraries(nanonet_static PRIVATE "-lws2_32")
endif()

