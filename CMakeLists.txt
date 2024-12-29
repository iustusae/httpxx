cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")

project(
  httpx
  VERSION 0.0.1
  LANGUAGES CXX)
enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/build)
add_subdirectory(${CMAKE_SOURCE_DIR}/tests ${CMAKE_SOURCE_DIR}/tests/build)
add_executable(server src/main.cc
       )

target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(server PUBLIC httpxx)
