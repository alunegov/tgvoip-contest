cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

option(WAVE_OUT "Also output as WAVE" OFF)

project(tgvoipcall LANGUAGES CXX)

if(MSVC)
    #find_package(OpenSSL)
    find_library(TGVOIP_LIB libtgvoip.lib)
else()
    find_library(CRYPTO_LIB libcrypto.a)
    find_library(TGVOIP_LIB libtgvoip.a HINTS ../_ext/libtgvoip/.libs)
endif()
find_path(TGVOIP_INCLUDE VoIPController.h HINTS ../_ext/libtgvoip)
if(NOT TARGET opusfile-reader)
    add_subdirectory(../opusfile-reader opusfile-reader)
endif()
add_subdirectory(../opusfile-writer opusfile-writer)
if(WAVE_OUT AND NOT TARGET wavefile-writer)
    add_subdirectory(../wavefile-writer wavefile-writer)
endif()

set(source_list "main.cpp")

add_executable(tgvoipcall ${source_list})

set_property(TARGET tgvoipcall PROPERTY CXX_STANDARD 14)
set_property(TARGET tgvoipcall PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(tgvoipcall
    PRIVATE ${TGVOIP_INCLUDE}
)

target_compile_definitions(tgvoipcall
    PRIVATE TGVOIP_USE_CALLBACK_AUDIO_IO
)
if(WAVE_OUT)
    target_compile_definitions(tgvoipcall
        PRIVATE TGVOIPCALL_WAVE_OUTPUT
    )
endif()

if(MSVC)
    target_compile_options(tgvoipcall
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(tgvoipcall
        PRIVATE -Wall -Wextra
    )
endif()

target_link_libraries(tgvoipcall
    PRIVATE ${TGVOIP_LIB}
    PRIVATE ${CRYPTO_LIB}
    PRIVATE opusfile-reader
    PRIVATE opusfile-writer
)
if(WAVE_OUT)
    target_link_libraries(tgvoipcall
        PRIVATE wavefile-writer
    )
endif()
if(MSVC)
    target_link_libraries(tgvoipcall
        PRIVATE ws2_32
        PRIVATE winmm
        PRIVATE Iphlpapi
    )
else()
    target_link_libraries(tgvoipcall
        PRIVATE dl
        PRIVATE pthread
    )
endif()
