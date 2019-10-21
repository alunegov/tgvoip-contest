cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(opusfile-reader LANGUAGES CXX)

find_library(OGG_LIB libogg.lib)
find_library(OPUS_LIB opus.lib)
find_library(OPUSFILE_LIB opusfile.lib)

set(source_list "reader.cpp" "reader.h")

add_library(opusfile-reader ${source_list})

set_property(TARGET opusfile-reader PROPERTY CXX_STANDARD 14)
set_property(TARGET opusfile-reader PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(opusfile-reader
    PUBLIC "."
    PRIVATE "../../../Work3/ogg/include"
    PRIVATE "../../../Work3/opus/include"
    PRIVATE "../../../Work3/opusfile/include"
)

if(MSVC)
    target_compile_options(opusfile-reader
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(opusfile-reader
        PRIVATE -Wall -Wextra
    )
endif()

target_link_libraries(opusfile-reader
    INTERFACE ${OGG_LIB}
    INTERFACE ${OPUS_LIB}
    INTERFACE ${OPUSFILE_LIB}
)

add_executable(opusfile-reader-test "reader_test.cpp")
target_include_directories(opusfile-reader-test PUBLIC $<TARGET_PROPERTY:opusfile-reader,INCLUDE_DIRECTORIES>)
if(MSVC)
    target_compile_options(opusfile-reader-test
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(opusfile-reader-test
        PRIVATE -Wall -Wextra
    )
endif()
target_link_libraries(opusfile-reader-test PUBLIC opusfile-reader)
