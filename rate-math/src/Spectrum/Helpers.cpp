#include <rate-math.h>

#include <cassert>
#include <cmath>

uint32_t FrequencyToIndex(RateFloat_t aFrequency, RateFloat_t aDf)
{
    assert(aDf > 0);
    
    return static_cast<uint32_t>(std::lround(aFrequency / aDf));
}

RateFloat_t IndexToFrequency(uint32_t aIndex, RateFloat_t aDf)
{
    assert(aDf > 0);
    
    return aIndex * aDf;
}
