cmake_minimum_required(VERSION 3.11)

project(libosmscout_route LANGUAGES C CXX)

set(BUILD_SHARED_LIBS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "win64")
else()
    set(ARCH_DIR "win32")
endif()

find_library(OSMSCOUT_LIB
    NAMES osmscout
    PATHS "${CMAKE_SOURCE_DIR}/lib/${ARCH_DIR}"
    NO_DEFAULT_PATH
)

add_library(osmscout_route SHARED
    src/main.c
    src/Router.cpp
    src/RouterContext.cpp
    src/RouterMulti.cpp
    src/RouterOptions.cpp
    src/RouterSingle.cpp
    src/RoutingProfileBuilder.cpp
)

set_target_properties(osmscout_route PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${ARCH_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/${ARCH_DIR}"
)

target_compile_options(osmscout_route PRIVATE -Wall)
target_link_options(osmscout_route PRIVATE -s -static -static-libgcc -static-libstdc++)

target_include_directories(osmscout_route PRIVATE include)

target_compile_definitions(osmscout_route PRIVATE BUILD_DLL)
target_compile_definitions(osmscout_route PRIVATE OSMSCOUT_STATIC)

if(OSMSCOUT_LIB)
    target_link_libraries(osmscout_route PRIVATE ${OSMSCOUT_LIB})
else()
    message(FATAL_ERROR "Could not find libosmscout.a for ${ARCH_DIR} in lib/${ARCH_DIR} directory!")
endif()
