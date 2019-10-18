cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tgvoipcall LANGUAGES CXX)

set(source_list "main.cpp")

add_executable(${PROJECT_NAME} ${source_list})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME}
    PRIVATE "../../../Work3/libtgvoip"
    )

target_compile_definitions(${PROJECT_NAME}
    PRIVATE TGVOIP_USE_CALLBACK_AUDIO_IO
    )

target_link_libraries(${PROJECT_NAME}
#    PRIVATE libtgvoip
    )
