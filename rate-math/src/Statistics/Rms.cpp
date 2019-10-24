#include <rate-math.h>

#include <cassert>
#include <cmath>

template <typename T>
static RateFloat_t Rms(T const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount, uint32_t aChannelIndex,
        RateFloat_t aMean)
{
    T const *const pointsEnd = aPoints + aPointsCount * aChannelsCount;
    RateFloat_t sqrSum = 0;
    
    T const *points = aPoints + aChannelIndex;
    while (points < pointsEnd) {
        RateFloat_t valueWithoutDc = *points - aMean;
        
        sqrSum += valueWithoutDc * valueWithoutDc;
        
        points += aChannelsCount;
    }
    
    return std::sqrt(sqrSum / aPointsCount);
}

RateFloat_t Rms_AdcRaw(AdcRaw_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount, uint32_t aChannelIndex,
        RateFloat_t aMean)
{
    return Rms<AdcRaw_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMean);
}

RateFloat_t Rms_RateFloat(RateFloat_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount,
        uint32_t aChannelIndex, RateFloat_t aMean)
{
    return Rms<RateFloat_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMean);
}

RateFloat_t RmsOnSpectrum(const RateFloat_t* aPoints, uint32_t aPointsCount)
{
    assert(aPoints != nullptr);

    RateFloat_t sqrSum = 0;

    for (uint_fast32_t i = 0; i < aPointsCount; i++) {
        sqrSum += aPoints[i] * aPoints[i];
    }

    return std::sqrt(sqrSum / 2);
}
