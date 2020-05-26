#include <rate-math.h>

#include <cassert>
#include <cmath>
#include <cstring>

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

RateFloat_t RmsOnSpectrum(const RateFloat_t* aPoints, uint32_t aPointsCount)
{
    assert(aPoints != nullptr);

    RateFloat_t sqrSum = 0;

    for (uint_fast32_t i = 0; i < aPointsCount; i++) {
        sqrSum += aPoints[i] * aPoints[i];
    }

    return std::sqrt(sqrSum / 2);
}

bool Cfft(RateFloat_t *x, RateFloat_t *y, uint32_t n, bool aDirect)
{
    assert(x != nullptr);
    assert(y != nullptr);
    assert(n >= 2);

    if ((x == nullptr) || (y == nullptr) || (n < 2)) {
        return false;
    }

    uint32_t e, f, i, i1, j, j1, k, l, m, o;
    RateFloat_t c, p, q, r, s, t, u, v, w, z;

    m = FloorLog2(n);
    assert(n == PowOf2(m));
    for (l = 1; l <= m; ++l) {
        e = 1 << (m + 1 - l);
        f = e / 2;
        u = 1;
        v = 0;
        z = static_cast<RateFloat_t>(M_PI) / f;
        c = std::cos(z);
        s = std::sin(z);
        if (!aDirect) {
            s *= -1;
        }
        for (j = 1; j <= f; ++j) {
            for (i = j; i <= n; i += e) {
                i1 = i - 1;
                o = i1 + f;
                p = x[i1] + x[o];
                q = y[i1] + y[o];
                r = x[i1] - x[o];
                t = y[i1] - y[o];
                x[o] = r*u - t*v;
                y[o] = t*u + r*v;
                x[i1] = p;
                y[i1] = q;
            }
            w = u*c - v*s;
            v = v*c + u*s;
            u = w;
        }
    }

    // èíâåðñèÿ áèòîâ:
    // 000000001 -> 100000000
    // 000000010 -> 010000000
    // 000000011 -> 110000000
    // ......................
    j = 1;
    for (i = 1; i <= n - 1; ++i) {
        if (i < j) {
            j1 = j - 1;
            i1 = i - 1;
            p = x[j1];
            q = y[j1];
            x[j1] = x[i1];
            y[j1] = y[i1];
            x[i1] = p;
            y[i1] = q;
        }
        k = n / 2;
        while (k < j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }

    if (aDirect) {
        for (k = 0; k < n; ++k) {
            x[k] /= n;
            y[k] /= n;
        }
    }

    // óäàëÿåì ïîñòîÿííóþ ñîñòàâëÿþùóþ
    //x[0] = y[0] = 0.0;

    return true;
}

bool Rfft(AdcRaw_t const *aSignal, RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aPointsCount,
          RateFloat_t aMean, bool aHamming)
{
    assert(aSignal != nullptr);
    assert(aSpectrumReal != nullptr);
    assert(aSpectrumImag != nullptr);
    assert(aPointsCount >= 2);

    // äåéñòâèòåëüíóþ ÷àñòü çàïîëíÿåì çíà÷åíèÿìè ñèãíàëà (âðåìåííÛå çíà÷åíèÿ)
    if (aHamming) {
        // Íàêëàäûâàåì îêíî Õåììèíãà
        // wiki: w(n) = 0.53836 - 0.46164 * cos(2 * PI * n / (N - 1)), n=0..N-1
        // Ìû äîïîëíèòåëüíî óìíîæàåì íà 1.86, ÷òîáû ïðèáëèçèòåëüíî ñîõðàíèòü ìîùíîñòü ñïåêòðà
        // Íàøà ôîðìóëà: w(n) = 1.86 * [0.54 - 0.46 * cos(2 * PI * n / (N - 1))], n=0..N-1

        const RateFloat_t d1 = 2 * static_cast<RateFloat_t>(M_PI) / (aPointsCount - 1);

        for (uint_fast32_t i = 0; i < aPointsCount; i++) {
            aSpectrumReal[i] = (aSignal[i] - aMean) * (static_cast<RateFloat_t>(1.0044) - static_cast<RateFloat_t>(0.8556) * std::cos(d1 * i));
        }
    }
    else {
        for (uint_fast32_t i = 0; i < aPointsCount; i++) {
            aSpectrumReal[i] = aSignal[i] - aMean;
        }
    }

    // ìíèìóþ ÷àñòü îáíóëÿåì
    memset(aSpectrumImag, 0, aPointsCount * sizeof(RateFloat_t));

    return Cfft(aSpectrumReal, aSpectrumImag, aPointsCount, true);
}
