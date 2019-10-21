cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(opusfile-writer LANGUAGES CXX)

find_library(OPUS_LIB opus.lib)
find_library(OPUSENC_LIB opusenc.lib)

set(source_list "writer.cpp" "writer.h")

add_library(opusfile-writer ${source_list})

set_property(TARGET opusfile-writer PROPERTY CXX_STANDARD 14)
set_property(TARGET opusfile-writer PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(opusfile-writer
    PUBLIC "."
    PRIVATE "../../../Work3/opus/include"
    PRIVATE "../../../Work3/libopusenc/include"
)

if(MSVC)
    target_compile_options(opusfile-writer
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(opusfile-writer
        PRIVATE -Wall -Wextra
    )
endif()

target_link_libraries(opusfile-writer
    INTERFACE ${OPUS_LIB}
    INTERFACE ${OPUSENC_LIB}
)

add_executable(opusfile-writer-test "writer_test.cpp")
target_include_directories(opusfile-writer-test PUBLIC $<TARGET_PROPERTY:opusfile-writer,INCLUDE_DIRECTORIES>)
if(MSVC)
    target_compile_options(opusfile-writer-test
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(opusfile-writer-test
        PRIVATE -Wall -Wextra
    )
endif()
target_link_libraries(opusfile-writer-test PUBLIC opusfile-writer)
