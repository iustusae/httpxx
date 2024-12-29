file(
        GLOB_RECURSE HTTPX_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cc" # Match all .cc files in subdirectories
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.hh" # Match all .hh files in subdirectories
)

include_directories(httpxx/util)
include_directories(../include)
add_library(httpxx STATIC ${HTTPX_SOURCES}
        ../include/tomlpp.hh
        httpxx/v2/core/response.h
)

target_include_directories(httpxx PUBLIC httpxx/util)
target_include_directories(httpxx PUBLIC ../include)
target_link_directories(httpxx PUBLIC httpxx/util)
target_link_directories(httpxx PUBLIC ../include)
