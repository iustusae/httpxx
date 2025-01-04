cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_FLAGS "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")

project(
  httpxx
  VERSION 2.0.0
  LANGUAGES CXX)
enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/lib/v2)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/v2 ${CMAKE_SOURCE_DIR}/lib/v2/build)

add_executable(example example/main.cc
       )

target_include_directories(example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/v2)
target_link_libraries(example PUBLIC httpxx_v2)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(example PRIVATE fmt::fmt-header-only)
