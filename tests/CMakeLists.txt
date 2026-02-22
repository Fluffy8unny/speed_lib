cmake_minimum_required(VERSION 3.14)

find_package(GTest REQUIRED)

enable_testing()

add_executable(test_speed test_speed.cpp)
target_include_directories(test_speed PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_speed PRIVATE GTest::gtest_main)

add_test(NAME speed_test COMMAND test_speed)
