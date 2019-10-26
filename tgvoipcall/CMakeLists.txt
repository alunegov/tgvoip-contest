cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(tgvoipcall LANGUAGES CXX)

if(MSVC)
    #find_package(OpenSSL)
    find_library(TGVOIP_LIB libtgvoip.lib)
else()
    find_library(CRYPTO_LIB libcrypto.a)
    find_library(TGVOIP_LIB libtgvoip.a HINTS ../_ext/libtgvoip/.libs)
endif()
find_path(TGVOIP_INCLUDE VoIPController.h HINTS ../_ext/libtgvoip)
add_subdirectory(../opusfile-reader opusfile-reader)
add_subdirectory(../opusfile-writer opusfile-writer)

set(source_list "main.cpp")

add_executable(tgvoipcall ${source_list})

set_property(TARGET tgvoipcall PROPERTY CXX_STANDARD 14)
set_property(TARGET tgvoipcall PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(tgvoipcall
    PRIVATE ${TGVOIP_INCLUDE}
)

target_compile_definitions(tgvoipcall
    PRIVATE TGVOIP_USE_CALLBACK_AUDIO_IO
    #PRIVATE TGVOIPCALL_WAVE_OUTPUT
)

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
    PRIVATE wavefile-writer
)
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
