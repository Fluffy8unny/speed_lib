cmake_minimum_required(VERSION 3.14)

add_executable(example example.cpp)
target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR}/include)
