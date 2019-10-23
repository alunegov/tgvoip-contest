cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(opusfile-writer LANGUAGES CXX)

if(MSVC)
    find_library(OPUS_LIB opus.lib)
    find_library(OPUSENC_LIB opusenc.lib)
else()
    find_library(OPUS_LIB libopus.a HINTS /usr/lib/x86_64-linux-gnu)
    find_library(OPUSENC_LIB libopusenc.a HINTS ../_ext/libopusenc/.libs)
endif()
find_path(OPUS_INCLUDE opus.h PATH_SUFFIXES opus)
find_path(OPUSENC_INCLUDE opusenc.h PATH_SUFFIXES opus)

set(source_list "opusfile-writer.cpp" "opusfile-writer.h")

add_library(opusfile-writer ${source_list})

set_property(TARGET opusfile-writer PROPERTY CXX_STANDARD 14)
set_property(TARGET opusfile-writer PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(opusfile-writer
    PUBLIC "."
    PRIVATE ${OPUS_INCLUDE}
    PRIVATE ${OPUSENC_INCLUDE}
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
    INTERFACE ${OPUSENC_LIB}
    INTERFACE ${OPUS_LIB}
)

add_subdirectory(../wavefile-reader wavefile-reader)

add_executable(opusfile-writer-test "opusfile-writer_test.cpp")
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
target_link_libraries(opusfile-writer-test PUBLIC opusfile-writer wavefile-reader)
