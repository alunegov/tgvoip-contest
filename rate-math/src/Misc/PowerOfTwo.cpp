#include <rate-math.h>

uint32_t FloorLog2(uint64_t n)
{
    uint32_t res = 0;
    for (; n > 1; n /= 2) {
        res++;
    }
    return res;
}

uint64_t PowOf2(uint32_t n)
{
    uint64_t res = 1;
    for (; n > 0; n--) {
        res *= 2;
    }
    return res;
}
