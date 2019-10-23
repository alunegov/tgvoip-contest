cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(rate-math LANGUAGES CXX)

set(source_list
    "include/rate-math.h"
    "src/Complex/Cmplx_AmplPhase.cpp"
    #"src/Misc/AdcFramesToChannels.cpp"
    #"src/Misc/AngleUnitConversion.cpp"
    #"src/Misc/CycledIter.cpp"
    "src/Misc/PowerOfTwo.cpp"
    #"src/Signal/Analysis/MarkerDetector.cpp"
    #"src/Spectrum/Analysis/HarmonicDetector.cpp"
    "src/Spectrum/Helpers.cpp"
    "src/Statistics/Excess.cpp"
    "src/Statistics/Mean.cpp"
    "src/Statistics/MinMaxMean.cpp"
    "src/Statistics/Rms.cpp"
    "src/Transform/Cfft.cpp"
    "src/Transform/Rfft.cpp"
)

add_library(rate-math ${source_list})

set_property(TARGET rate-math PROPERTY CXX_STANDARD 14)
set_property(TARGET rate-math PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(rate-math
    PUBLIC "include"
)

if(MSVC)
    target_compile_options(rate-math
        PRIVATE /W4 /MTd
    )
else()
    target_compile_options(rate-math
        PRIVATE -Wall -Wextra
    )
endif()
