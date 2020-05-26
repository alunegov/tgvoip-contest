cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(wavefile-reader LANGUAGES CXX)

set(source_list
    "wavefile-reader.cpp"
    "wavefile-reader.h"
)

add_library(wavefile-reader ${source_list})

set_property(TARGET wavefile-reader PROPERTY CXX_STANDARD 14)
set_property(TARGET wavefile-reader PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(wavefile-reader
    PUBLIC "."
)

if(MSVC)
    target_compile_definitions(wavefile-reader
        PRIVATE _CRT_SECURE_NO_WARNINGS
    )
endif()

if(MSVC)
    target_compile_options(wavefile-reader
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(wavefile-reader
        PRIVATE -Wall -Wextra
    )
endif()
