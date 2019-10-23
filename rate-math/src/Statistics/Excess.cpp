#include <rate-math.h>

#include <cassert>

template <typename T>
static RateFloat_t Excess(T const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount, uint32_t aChannelIndex,
        RateFloat_t aMean)
{
    assert(aPoints != nullptr);
    assert(aPointsCount > 0);
    assert(aChannelsCount > 0);

    T const *const pointsEnd = aPoints + aPointsCount * aChannelsCount;

    RateFloat_t quadSum = 0;
    RateFloat_t sqrSum = 0;

    T const *points = aPoints + aChannelIndex;
    while (points < pointsEnd) {
        RateFloat_t d3 = *points - aMean;
        d3 = d3 * d3;

        quadSum += d3 * d3;
        sqrSum += d3;

        points += aChannelsCount;
    }

    assert(sqrSum > 0);

    return quadSum * aPointsCount / (sqrSum * sqrSum) - 3;
}

RateFloat_t Excess_AdcRaw(AdcRaw_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount,
        uint32_t aChannelIndex, RateFloat_t aMean)
{
    return Excess<AdcRaw_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMean);
}

RateFloat_t Excess_RateFloat(RateFloat_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount,
        uint32_t aChannelIndex, RateFloat_t aMean)
{
    return Excess<RateFloat_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMean);
}
