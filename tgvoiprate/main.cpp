#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include <opusfile-reader.h>
#include <rate-math.h>

constexpr auto RegFreq{48000.0};

template <typename T>
void dumpToCsv(const T* buf, const size_t size, const std::string& fileName) {
    std::ofstream file{fileName};
    for (size_t i = 0; i < size; i++) {
        file << i << ";" << buf[i] << std::endl;
    }
}

void printUsage();
std::pair<std::string, std::string> parseArgs(int argc, const char** argv);
std::vector<double_t> calcBandsPower(const std::vector<int16_t>& signal, std::vector<double_t>& realBuf, std::vector<double_t>& imagBuf);
double_t variance(const std::vector<double_t>& v);
double_t calcRate(double_t variance);

int main(int argc, const char** argv) {
    try {
        const auto conf = parseArgs(argc, argv);

        if (conf.first.empty() || conf.second.empty()) {
            printUsage();
            return -1;
        }

        OpusFileReader refR;
        OpusFileReader testR;

        if (!refR.Open(conf.first)) {
            std::cerr << "Error opening " << conf.first << std::endl;
            return -2;
        }
        if (!testR.Open(conf.second)) {
            std::cerr << "Error opening " << conf.second << std::endl;
            return -3;
        }

        //const auto BufLen{2048}; // ~ 42 ms
        //const auto BufLen{4096}; // ~ 85 ms
        const auto BufLen{8192}; // ~ 170 ms
        //const auto BufLen{16384}; // ~ 340 ms
        //const auto BufLen{32768}; // ~ 680 ms
        //const auto BufLen{65536}; // ~ 1365 ms

        std::vector<int16_t> refBuf(BufLen);
        std::vector<int16_t> testBuf(BufLen);

        std::vector<double_t> tmpRealBuf(BufLen);
        std::vector<double_t> tmpImagBuf(BufLen);

        std::vector<std::vector<double_t>> refBands;
        std::vector<std::vector<double_t>> testBands;

        std::vector<double_t> refExcs;
        std::vector<double_t> testExcs;

        //size_t counter{0};

        while (refR.Read(refBuf.data(), BufLen) && testR.Read(testBuf.data(), BufLen)) {
            //dumpToCsv(refBuf.data(), BufLen, std::to_string(counter) + "_refBuf.csv");
            //dumpToCsv(testBuf.data(), BufLen, std::to_string(counter) + "_testBuf.csv");

            const auto refFrameBands = calcBandsPower(refBuf, tmpRealBuf, tmpImagBuf);
            const auto testFrameBands = calcBandsPower(testBuf, tmpRealBuf, tmpImagBuf);
            if (refFrameBands.empty() || testFrameBands.empty()) {
                // fft error
                continue;
            }
            assert(refFrameBands.size() == testFrameBands.size());

            refBands.emplace_back(refFrameBands);
            testBands.emplace_back(testFrameBands);

            //
            const auto refExc = Excess_AdcRaw(refBuf.data(), BufLen, 1, 0, 0);
            const auto testExc = Excess_AdcRaw(testBuf.data(), BufLen, 1, 0, 0);

            refExcs.emplace_back(refExc);
            testExcs.emplace_back(testExc);

            //counter++;
        }

        // 7*BufLen ~ 1200 ms
        std::vector<double_t> shiftedTotalVariance;

        // shifting test start to one frame and
        // variance of all frames
        for (uint_fast8_t frameShift = 0; frameShift < 7; frameShift++) {
            std::vector<double_t> framesVariance(refBands.size());

            // variance of diff between ref and test in all bands
            assert(refBands.size() == testBands.size());
            for (size_t i = 0; i < refBands.size() - frameShift; i++) {
                std::vector<double_t> frameDeltas(refBands[i].size());

                // diff of each band between ref and test
                assert(refBands[i].size() == testBands[i].size());
                for (size_t j = 0; j < refBands[i].size(); j++) {
                    // TODO: coeff for speech freqs (60-2000)
                    frameDeltas[j] = abs(refBands[i][j] - testBands[frameShift + i][j]);
                }

                framesVariance[i] = variance(frameDeltas);
            }

            const auto totalVariance = variance(framesVariance);
#ifndef NDEBUG
            std::cout << " var " << totalVariance;
            const auto totalMean = Mean_RateFloat(framesVariance.data(), framesVariance.size());
            std::cout << " mean " << totalMean;
            std::cout << " rate " << calcRate(totalVariance);
            std::cout << " at frameShift " << frameShift * BufLen / RegFreq * 1000 << " ms" << std::endl;
#endif

            shiftedTotalVariance.emplace_back(totalVariance);
        }

        // 7*BufLen ~ 1200 ms
        std::vector<double_t> shiftedTotalVariance2;

        // shifting test start to one frame and
        // variance of all frames
        for (uint_fast8_t frameShift = 0; frameShift < 7; frameShift++) {
            std::vector<double_t> frameDeltas(refExcs.size());

            // diff of each band between ref and test
            assert(refBands.size() == testBands.size());
            for (size_t i = 0; i < refExcs.size() - frameShift; i++) {
                frameDeltas[i] = abs(refExcs[i] - testExcs[frameShift + i]);
            }

            const auto totalVariance = variance(frameDeltas);
#ifndef NDEBUG
            std::cout << " var " << totalVariance;
            const auto totalMean = Mean_RateFloat(frameDeltas.data(), frameDeltas.size());
            std::cout << " mean " << totalMean;
            std::cout << " rate " << calcRate(totalVariance);
            std::cout << " at frameShift " << frameShift * BufLen / RegFreq * 1000 << " ms" << std::endl;
#endif

            shiftedTotalVariance2.emplace_back(totalVariance);
        }

        const auto minTotalVariance = std::min_element(shiftedTotalVariance.begin(), shiftedTotalVariance.end());
        assert(minTotalVariance != shiftedTotalVariance.end());
        std::cout << calcRate(*minTotalVariance) << std::endl;

        // TODO: call dtors of refR, testR to catch their errors?

        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -13;
    }
}

void printUsage() {
    std::cout << "Usage: tgvoiprate /path/to/sound_A.opus /path/to/sound_output_A.opus" << std::endl;
}

std::pair<std::string, std::string> parseArgs(int argc, const char** argv) {
#ifndef NDEBUG
    if (argc == 1) {
        printUsage();
        std::cout << "Will use defaults for now" << std::endl;

        return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "out_caller_sample06_b05e9d0ca9fa03bc46191299c1bae645.ogg");  // same file with delay
        //return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg");  // same file
        //return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "sample06_b05e9d0ca9fa03bc46191299c1bae645.ogg");  // diff files
        //return std::make_pair("sample05_ff63f34c691af48ef285649054ab4906.ogg", "out_callee_sample05_ff63f34c691af48ef285649054ab4906.ogg");  // bad
        //return std::make_pair("sample05_0bb3646f15e8dc61f525f40f2884de57.ogg", "out_caller_sample05_0bb3646f15e8dc61f525f40f2884de57.ogg");  // bad

    }
#endif

    if (argc > 2) {
        return std::make_pair(std::string{argv[1]}, std::string{argv[2]});
    } else {
        return std::make_pair("", "");
    }
}

std::vector<double_t> calcBandsPower(const std::vector<int16_t>& signal, std::vector<double_t>& realBuf, std::vector<double_t>& imagBuf) {
    const auto len{signal.size()};
    assert(len == (size_t)PowOf2(FloorLog2(len)));  // len should be a power of 2
    const auto mean{0.0};  // assume where's no DC
    const auto spec_len{len / 2};  // spectrum amplitudes are symmetrical - will use only first half

    std::vector<double_t> res;

    if (!Rfft(signal.data(), realBuf.data(), imagBuf.data(), len, mean, false)) {
#ifndef NDEBUG
        std::cerr << "Rfft error" << std::endl;
#endif
        return res;
    }

    Cmplx_Ampl(realBuf.data(), imagBuf.data(), len, spec_len);
    // TODO: magic?
    for (size_t i = 0; i < spec_len; i++) {
        realBuf[i] = realBuf[i] * 1000;
    }
    //dumpToCsv(realBuf.data(), spec_len, "realBuf.csv");

    // df = 1 / (dt * PointsCount) = RegFreq / PointsCount
    const auto df{RegFreq / len};

    // 24000 (RegFreq / 2) / 200 ~ 120 bands
    const auto pointsInBand{FrequencyToIndex(100, df)};

    for (size_t i = 0; i < spec_len; i += pointsInBand) {
        if (IndexToFrequency(i, df) > 3800) {
            //break;
        }

        // limit last band to spectrum highest freq
        uint32_t bandLen;
        if ((i + pointsInBand) > spec_len) {
            bandLen = spec_len - i;
        } else {
            bandLen = pointsInBand;
        }

        const auto rms = RmsOnSpectrum(&realBuf[i], bandLen);
        res.emplace_back(rms);
    }

    return res;
}

double_t variance(const std::vector<double_t>& v) {
    assert(!v.empty());

    const auto mean = Mean_RateFloat(v.data(), v.size());

    auto sqrSum{0.0};
    for (double_t i : v) {
        sqrSum += pow(i - mean, 2);
    }
    return sqrt(sqrSum / v.size());
}

double_t calcRate(double_t variance) {
    // 5 - (var - 0.7), depends on magic in calcBandsPower
    return fmax(5.0 - fmax(variance - 0.7, 0.0), 1.0);
}
