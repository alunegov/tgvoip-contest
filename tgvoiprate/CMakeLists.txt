cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tgvoiprate LANGUAGES CXX)

add_subdirectory(../opusfile-reader opusfile-reader)

set(source_list "main.cpp")

add_executable(tgvoiprate ${source_list})

set_property(TARGET tgvoiprate PROPERTY CXX_STANDARD 14)
set_property(TARGET tgvoiprate PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(tgvoiprate
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(tgvoiprate
        PRIVATE -Wall -Wextra
    )
endif()

target_link_libraries(tgvoiprate
    PRIVATE opusfile-reader
)
