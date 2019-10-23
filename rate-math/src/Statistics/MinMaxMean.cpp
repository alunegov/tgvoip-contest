#include <rate-math.h>

#include <cassert>

template <typename T>
static void MinMaxMean(T const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount, uint32_t aChannelIndex,
        T *aMin, T *aMax, RateFloat_t *aMean)
{
    assert(aPoints != nullptr);
    assert(aPointsCount > 0);
    assert(aChannelsCount > 0);
    assert(aMin != nullptr);
    assert(aMax != nullptr);
    assert(aMean != nullptr);

    T const *const pointsEnd = aPoints + aPointsCount * aChannelsCount;

    T const *points = aPoints + aChannelIndex;
    T pointValue = *points;
    *aMin = pointValue;
    *aMax = pointValue;
    RateFloat_t sum = static_cast<RateFloat_t>(pointValue);

    points += aChannelsCount;
    while (points < pointsEnd) {
        pointValue = *points;

        if (*aMin > pointValue) {
            *aMin = pointValue;
        }

        if (*aMax < pointValue) {
            *aMax = pointValue;
        }

        sum += static_cast<RateFloat_t>(pointValue);

        points += aChannelsCount;
    }

    *aMean = sum / aPointsCount;
}

void MinMaxMean_AdcRaw(AdcRaw_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount, uint32_t aChannelIndex,
        AdcRaw_t *aMin, AdcRaw_t *aMax, RateFloat_t *aMean)
{
    MinMaxMean<AdcRaw_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMin, aMax, aMean);
}

void MinMaxMean_RateFloat(RateFloat_t const *aPoints, uint32_t aPointsCount, uint32_t aChannelsCount,
        uint32_t aChannelIndex, RateFloat_t *aMin, RateFloat_t *aMax, RateFloat_t *aMean)
{
    MinMaxMean<RateFloat_t>(aPoints, aPointsCount, aChannelsCount, aChannelIndex, aMin, aMax, aMean);
}
