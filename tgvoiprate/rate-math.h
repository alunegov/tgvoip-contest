#pragma once

#if defined WIN32 || defined _WIN32
#   define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define AdcRaw_t int16_t
#define RateFloat_t double_t

#ifdef __cplusplus
extern "C" {
#endif

// Complex

void Cmplx_Ampl(RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aCount, uint32_t aProcessingCount);

// Misc

// Returns the largest i such that 2^i <= n.
uint32_t FloorLog2(uint64_t n);

uint64_t PowOf2(uint32_t n);

// Spectrum

uint32_t FrequencyToIndex(RateFloat_t aFrequency, RateFloat_t aDf);

RateFloat_t IndexToFrequency(uint32_t aIndex, RateFloat_t aDf);

// Statistics

RateFloat_t Mean_AdcRaw(AdcRaw_t const *aPoints, uint32_t aPointsCount);

RateFloat_t Mean_RateFloat(RateFloat_t const *aPoints, uint32_t aPointsCount);

RateFloat_t RmsOnSpectrum(const RateFloat_t* aPoints, uint32_t aPointsCount);

// Transform

bool Cfft(RateFloat_t *x, RateFloat_t *y, uint32_t n, bool aDirect);

bool Rfft(AdcRaw_t const *aSignal, RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aPointsCount,
        RateFloat_t aMean, bool aHamming);

#ifdef __cplusplus
}
#endif
