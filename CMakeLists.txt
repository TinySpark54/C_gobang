cmake_minimum_required(VERSION 4.0)
project(C_gobang)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(${PROJECT_NAME}
        gobanglib.cpp
        stack.cpp
        gobang.cpp)

add_executable(graphic graphic_test.cpp
        linked_list.h
        gobang.cpp)
add_executable(test test.cpp)
