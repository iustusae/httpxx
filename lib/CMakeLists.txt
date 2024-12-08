file(
        GLOB_RECURSE HTTPX_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cc" # Match all .cc files in subdirectories
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp" # Match all .cpp files in subdirectories
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.hh" # Match all .hh files in subdirectories
)

add_library(httpxx STATIC ${HTTPX_SOURCES}
        core/http_object.cpp
        ../include/tomlpp.hh
)
