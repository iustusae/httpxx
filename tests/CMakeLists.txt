
cmake_minimum_required(VERSION 3.29)
project(httpxx_test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        CMAKE_ARGS
        "-DCMAKE_MINIMUM_REQUIRED_VERSION=3.20"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)



add_executable(httpxx_testing test_main.cpp
        tests/ConfigTest.h
)
target_link_libraries(httpxx_testing gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(httpxx_testing)

target_include_directories(httpxx_testing PUBLIC ${CMAKE_SOURCE_DIR}/tests)
target_link_directories(httpxx_testing PUBLIC ${CMAKE_SOURCE_DIR}/tests)
