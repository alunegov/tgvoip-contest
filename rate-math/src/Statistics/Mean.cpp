#include <rate-math.h>

#include <cassert>

template <typename T>
RateFloat_t Mean(T const *aPoints, uint32_t aPointsCount)
{
    assert(aPoints != nullptr);
    assert(aPointsCount > 0);
    
    RateFloat_t aver = 0;
    
    for (uint_fast32_t i1 = 0; i1 < aPointsCount; i1++) {
        aver += aPoints[i1];
    }
    
    return aver / aPointsCount;
}

RateFloat_t Mean_AdcRaw(AdcRaw_t const *aPoints, uint32_t aPointsCount)
{
    return Mean<AdcRaw_t>(aPoints, aPointsCount);
}

RateFloat_t Mean_RateFloat(RateFloat_t const *aPoints, uint32_t aPointsCount)
{
    return Mean<RateFloat_t>(aPoints, aPointsCount);
}
