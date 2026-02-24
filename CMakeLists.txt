cmake_minimum_required(VERSION 3.14)

project(speed_lib LANGUAGES CXX)

# This repo is header-only; still set a standard for the tests.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
