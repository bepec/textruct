cmake_minimum_required(VERSION 2.6)

project(unittest)

add_definitions(-DBOOST_TEST_DYN_LINK)
add_definitions(-std=c++11)

set(SOURCE_EXE test.cpp)

add_executable(test-textruct ${SOURCE_EXE})

target_link_libraries(test-textruct boost_unit_test_framework)
