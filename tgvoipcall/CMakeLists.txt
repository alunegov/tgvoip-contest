cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tgvoipcall LANGUAGES CXX)

find_package(OpenSSL)
if(MSVC)
    find_library(TGVOIP_LIB libtgvoip.lib)
else()
    find_library(TGVOIP_LIB libtgvoip.a)
endif()
add_subdirectory(../opusfile-reader opusfile-reader)
add_subdirectory(../opusfile-writer opusfile-writer)

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

if(MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE -Wall -Wextra
    )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${OPENSSL_CRYPTO_LIBRARY}
    #PRIVATE ${TGVOIP_LIB}
    PRIVATE opusfile-reader
    PRIVATE opusfile-writer
)
if(MSVC)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ws2_32
        PRIVATE winmm
        PRIVATE Iphlpapi
        #PRIVATE opus
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PRIVATE pthread
        PRIVATE dl
        PRIVATE opus
    )
endif()