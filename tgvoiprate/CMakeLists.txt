cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tgvoiprate LANGUAGES CXX)

add_subdirectory(../opusfile-reader opusfile-reader)
add_subdirectory(../rate-math rate-math)

set(source_list "main.cpp")

add_executable(tgvoiprate ${source_list})

set_property(TARGET tgvoiprate PROPERTY CXX_STANDARD 14)
set_property(TARGET tgvoiprate PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(tgvoiprate
    PRIVATE "."
)

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
    PRIVATE rate-math
)
