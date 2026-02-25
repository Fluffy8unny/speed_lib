cmake_minimum_required(VERSION 3.14)

find_package(GTest REQUIRED)

enable_testing()

add_executable(test_arithmetic test_arithmetic.cpp)
target_include_directories(test_arithmetic PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_arithmetic PRIVATE GTest::gtest_main)
add_test(NAME arithmetic_test COMMAND test_arithmetic)

add_executable(test_conversion test_conversion.cpp)
target_include_directories(test_conversion PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_conversion PRIVATE GTest::gtest_main)
add_test(NAME conversion_test COMMAND test_conversion)

add_executable(test_formating test_formating.cpp)
target_include_directories(test_formating PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_formating PRIVATE GTest::gtest_main)
add_test(NAME formating_test COMMAND test_formating)

add_executable(test_literals test_literals.cpp)
target_include_directories(test_literals PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_literals PRIVATE GTest::gtest_main)  
add_test(NAME literals_test COMMAND test_literals)

add_executable(test_time_length test_time_length.cpp)
target_include_directories(test_time_length PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_time_length PRIVATE GTest::gtest_main)
add_test(NAME time_length_test COMMAND test_time_length)
