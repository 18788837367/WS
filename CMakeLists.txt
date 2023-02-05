cmake_minimum_required(VERSION 3.19)

project(WS VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BINARY_ROOT_DIR "${WS_ROOT_DIR}/bin")

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/W3")
endif()

add_subdirectory(graphical_api)
add_subdirectory(mini_engine)