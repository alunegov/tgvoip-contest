#include <rate-math.h>

#include <cassert>
#include <cmath>

void Cmplx_Ampl(RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aCount, uint32_t aProcessingCount)
{
    assert(aSpectrumReal != nullptr);
    assert(aSpectrumImag != nullptr);
    assert(aCount >= 2);
    assert(aCount >= aProcessingCount);

    const auto spc = static_cast<RateFloat_t>(1.0) / aCount / 2;

    for (uint_fast32_t i = 0; i < aProcessingCount; i++) {
        *aSpectrumReal = spc * std::sqrt((*aSpectrumReal) * (*aSpectrumReal) + (*aSpectrumImag) * (*aSpectrumImag));

        aSpectrumReal++;
        aSpectrumImag++;
    }
}

void Cmplx_AmplPhase(RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aCount, uint32_t aProcessingCount)
{
    assert(aSpectrumReal != nullptr);
    assert(aSpectrumImag != nullptr);
    assert(aCount >= 2);
    assert(aCount >= aProcessingCount);

    const auto spc = static_cast<RateFloat_t>(1.0) / aCount / 2;

    for (uint_fast32_t i = 0; i < aProcessingCount; i++) {
        const auto phase = std::atan2(*aSpectrumImag, *aSpectrumReal);

        *aSpectrumReal = spc * std::sqrt((*aSpectrumReal) * (*aSpectrumReal) + (*aSpectrumImag) * (*aSpectrumImag));
        *aSpectrumImag = phase;
        
        aSpectrumReal++;
        aSpectrumImag++;
    }
}
