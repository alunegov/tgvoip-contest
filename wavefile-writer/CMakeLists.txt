cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(wavefile-writer LANGUAGES CXX)

set(source_list "wavefile-writer.cpp" "wavefile-writer.h")

add_library(wavefile-writer ${source_list})

set_property(TARGET wavefile-writer PROPERTY CXX_STANDARD 14)
set_property(TARGET wavefile-writer PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(wavefile-writer
    PUBLIC "."
)

if(MSVC)
    target_compile_options(wavefile-writer
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(wavefile-writer
        PRIVATE -Wall -Wextra
    )
endif()
