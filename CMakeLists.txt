cmake_minimum_required(VERSION 4.0)
project(C_gobang)

set(CMAKE_CXX_STANDARD 14)




add_executable(${PROJECT_NAME}
        gobang.cpp
        linked_list.cpp
        main.cpp
        graphic_test.cpp)

add_executable(graphic graphic_test.cpp)
