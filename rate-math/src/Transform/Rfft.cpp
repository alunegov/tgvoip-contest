#include <rate-math.h>

#include <cassert>
#include <cmath>
#include <cstring>

bool Rfft(AdcRaw_t const *aSignal, RateFloat_t *aSpectrumReal, RateFloat_t *aSpectrumImag, uint32_t aPointsCount,
        RateFloat_t aMean, bool aHamming)
{
    assert(aSignal != nullptr);
    assert(aSpectrumReal != nullptr);
    assert(aSpectrumImag != nullptr);
    assert(aPointsCount >= 2);
    
    // �������������� ����� ��������� ���������� ������� (��������� ��������)
    if (aHamming) {
        // ����������� ���� ��������
        // wiki: w(n) = 0.53836 - 0.46164 * cos(2 * PI * n / (N - 1)), n=0..N-1
        // �� ������������� �������� �� 1.86, ����� �������������� ��������� �������� �������
        // ���� �������: w(n) = 1.86 * [0.54 - 0.46 * cos(2 * PI * n / (N - 1))], n=0..N-1
        
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
    
    // ������ ����� ��������
    memset(aSpectrumImag, 0, aPointsCount * sizeof(RateFloat_t));
    
    return Cfft(aSpectrumReal, aSpectrumImag, aPointsCount, true);
}
