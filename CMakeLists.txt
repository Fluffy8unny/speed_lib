cmake_minimum_required(VERSION 3.14)

project(speed_lib LANGUAGES CXX)

# This repo is header-only; still set a standard for the tests.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SPEED_LIB_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)

if(SPEED_LIB_ENABLE_STRICT_WARNINGS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
  elseif(MSVC)
    add_compile_options(/W4)
  endif()
endif()

include(CTest)

add_subdirectory(examples)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
