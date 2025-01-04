
file(
        GLOB_RECURSE HTTPX_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/*.hh" # Match all .hh files in subdirectories
)

include_directories(../include/)
add_library(httpxx_v2 STATIC ${HTTPX_SOURCES} )
set_target_properties(httpxx_v2 PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(httpxx_v2 PUBLIC ../include)
target_link_directories(httpxx_v2 PUBLIC ../include)
