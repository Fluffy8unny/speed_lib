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

add_executable(test_compile_time test_compile_time.cpp)
target_include_directories(test_compile_time PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME compile_time_test COMMAND test_compile_time)

add_executable(test_comparison test_comparison.cpp)
target_include_directories(test_comparison PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_comparison PRIVATE GTest::gtest_main)
add_test(NAME comparison_test COMMAND test_comparison)
