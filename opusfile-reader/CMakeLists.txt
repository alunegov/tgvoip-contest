cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(opusfile-reader LANGUAGES CXX)

if(MSVC)
    find_library(OGG_LIB ogg.lib)
    find_library(OPUS_LIB opus.lib)
    find_library(OPUSFILE_LIB opusfile.lib)
else()
    find_library(OGG_LIB libogg.a HINTS /usr/lib/x86_64-linux-gnu)
    find_library(OPUS_LIB libopus.a HINTS /usr/lib/x86_64-linux-gnu)
    find_library(OPUSFILE_LIB libopusfile.a HINTS /usr/lib)
endif()
find_path(OGG_INCLUDE ogg.h PATH_SUFFIXES ogg)
find_path(OPUS_INCLUDE opus.h PATH_SUFFIXES opus)
find_path(OPUSFILE_INCLUDE opusfile.h PATH_SUFFIXES opus)

set(source_list "opusfile-reader.cpp" "opusfile-reader.h")

add_library(opusfile-reader ${source_list})

set_property(TARGET opusfile-reader PROPERTY CXX_STANDARD 14)
set_property(TARGET opusfile-reader PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(opusfile-reader
    PUBLIC "."
    PRIVATE ${OGG_INCLUDE}
    PRIVATE ${OGG_INCLUDE}/..
    PRIVATE ${OPUS_INCLUDE}
    PRIVATE ${OPUSFILE_INCLUDE}
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
    INTERFACE ${OPUSFILE_LIB}
    INTERFACE ${OPUS_LIB}
    INTERFACE ${OGG_LIB}
)

add_subdirectory(../wavefile-writer wavefile-writer)

add_executable(opusfile-reader-test "opusfile-reader_test.cpp")
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
target_link_libraries(opusfile-reader-test PUBLIC opusfile-reader wavefile-writer)
