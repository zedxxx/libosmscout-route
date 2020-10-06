cmake_minimum_required(VERSION 3.11)

project(libosmscout_route)

set(BUILD_SHARED_LIBS 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s -static -static-libgcc -static-libstdc++")

add_definitions(-DBUILD_DLL)
add_definitions(-DOSMSCOUT_STATIC)

include_directories(include)
link_directories(lib)

add_library(libosmscout_route
        src/main.c
        src/Router.cpp
        src/Router.h
        src/RouterContext.h)

target_link_libraries(libosmscout_route osmscout)