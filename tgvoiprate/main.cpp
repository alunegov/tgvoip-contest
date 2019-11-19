#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <opusfile-reader.h>
#include <rate-math.h>

constexpr double_t RegFreq{48000.0};

//constexpr size_t BufLen{512}; // ~ 10 ms
//constexpr size_t BufLen{1024}; // ~ 21 ms
//constexpr size_t BufLen{2048}; // ~ 42 ms
constexpr size_t BufLen{4096}; // ~ 85 ms
//constexpr size_t BufLen{8192}; // ~ 170 ms
//constexpr size_t BufLen{16384}; // ~ 340 ms
//constexpr size_t BufLen{32768}; // ~ 680 ms
//constexpr size_t BufLen{65536}; // ~ 1365 ms

constexpr double_t BandWidth{300.0};

template <typename T>
void dumpToCsv(const T* buf, const size_t size, double_t dx, const std::string& fileName) {
    std::ofstream file{fileName};
    for (size_t i = 0; i < size; i++) {
        file << i * dx << "\t" << buf[i] << "\t" << i << std::endl;
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

        std::vector<int16_t> refBuf(BufLen);
        std::vector<int16_t> testBuf(BufLen);

        std::vector<double_t> tmpRealBuf(BufLen);
        std::vector<double_t> tmpImagBuf(BufLen);

        std::vector<std::vector<double_t>> refBands;
        std::vector<std::vector<double_t>> testBands;

        size_t counter{0};

        while (refR.Read(refBuf.data(), BufLen) && testR.Read(testBuf.data(), BufLen)) {
#ifndef NDEBUG
            //dumpToCsv(refBuf.data(), BufLen, 1 / RegFreq, std::to_string(counter) + "_refBuf.csv");
            //dumpToCsv(testBuf.data(), BufLen, 1 / RegFreq, std::to_string(counter) + "_testBuf.csv");
#endif

            const auto refFrameBands = calcBandsPower(refBuf, tmpRealBuf, tmpImagBuf);
#ifndef NDEBUG
            //dumpToCsv(tmpRealBuf.data(), BufLen / 2, RegFreq / BufLen, std::to_string(counter) + "_refAmpl.csv");
            //dumpToCsv(refFrameBands.data(), refFrameBands.size(), BandWidth, std::to_string(counter) + "_refBands.csv");
#endif
            const auto testFrameBands = calcBandsPower(testBuf, tmpRealBuf, tmpImagBuf);
#ifndef NDEBUG
            //dumpToCsv(tmpRealBuf.data(), BufLen / 2, RegFreq / BufLen, std::to_string(counter) + "_testAmpl.csv");
            //dumpToCsv(testFrameBands.data(), testFrameBands.size(), BandWidth, std::to_string(counter) + "_testBands.csv");
#endif
            if (refFrameBands.empty() || testFrameBands.empty()) {
                // fft error
                continue;
            }
            assert(refFrameBands.size() == testFrameBands.size());

            refBands.emplace_back(refFrameBands);
            testBands.emplace_back(testFrameBands);

            counter++;
        }

        std::vector<double_t> shiftedTotalVar;

        // shift ~ 1000 ms
        const size_t FrameShiftCount{(size_t)(llround(1.0 / (BufLen / RegFreq)))};

        // shifting test start to one frame and
        // variance of all frames
        for (uint_fast8_t frameShift = 0; frameShift < FrameShiftCount; frameShift++) {
            std::vector<double_t> framesMean(refBands.size() - frameShift);
            std::vector<double_t> framesVar(refBands.size() - frameShift);

            // variance of diff between ref and test in all bands
            assert(refBands.size() == testBands.size());
            for (size_t i = 0; i < refBands.size() - frameShift; i++) {
                std::vector<double_t> frameDeltas(refBands[i].size());

                // diff of each band between ref and test
                assert(refBands[i].size() == testBands[i].size());
                for (size_t j = 0; j < refBands[i].size(); j++) {
                    // TODO: coeff for speech freqs (60-2000)
                    //frameDeltas[j] = abs(refBands[i][j] - testBands[frameShift + i][j]);
                    frameDeltas[j] = refBands[i][j] / testBands[frameShift + i][j];
                }
                //dumpToCsv(frameDeltas.data(), frameDeltas.size(), BandWidth, std::to_string(frameShift) + "_f" + std::to_string(i) + "_delta.csv");

                framesMean[i] = Mean_RateFloat(frameDeltas.data(), frameDeltas.size());
                framesVar[i] = variance(frameDeltas);
            }
#ifndef NDEBUG
            //dumpToCsv(framesMean.data(), framesMean.size(), BufLen / RegFreq, std::to_string(frameShift) + "_mean.csv");
            //dumpToCsv(framesVariance.data(), framesVariance.size(), BufLen / RegFreq, std::to_string(frameShift) + "_var.csv");
#endif

            double_t totalMean = Mean_RateFloat(framesMean.data(), framesMean.size());
            double_t totalVar = variance(framesMean);
            std::cout << " var " << totalVar;
            std::cout << " mean " << totalMean;

            const double_t threshold_for_mean{totalMean + 0.3 * totalVar};

            std::cout << " skipping all above " << threshold_for_mean;
            std::vector<double_t> framesMean_;
            std::vector<double_t> framesVar_;
            for (size_t i = 0; i < framesMean.size(); i++) {
                if (framesMean[i] < threshold_for_mean) {
                    framesMean_.emplace_back(framesMean[i]);
                }
            }

            totalMean = Mean_RateFloat(framesMean_.data(), framesMean_.size());
            totalVar = variance(framesMean_);
#ifndef NDEBUG
            std::cout << " var_ " << totalVar;
            std::cout << " mean_ " << totalMean;
            std::cout << " rate " << calcRate(totalVar);
            std::cout << " at frameShift " << frameShift * BufLen / RegFreq * 1000 << " ms" << std::endl;
#endif

            shiftedTotalVar.emplace_back(totalVar);
        }

        const auto minTotalVar = std::min_element(shiftedTotalVar.begin(), shiftedTotalVar.end());
        assert(minTotalVar != shiftedTotalVar.end());
        std::cout << std::setprecision(5) << calcRate(*minTotalVar) << std::endl;

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

        //return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "out_caller_sample06_b05e9d0ca9fa03bc46191299c1bae645.ogg");  // same file with delay
        //return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg");  // same file
        //return std::make_pair("sample05_066a3936b4ebc1ca0c3b9e5d4e061e4b.ogg", "sample06_b05e9d0ca9fa03bc46191299c1bae645.ogg");  // diff files
        //return std::make_pair("sample05_ff63f34c691af48ef285649054ab4906.ogg", "out_callee_sample05_ff63f34c691af48ef285649054ab4906.ogg");  // bad
        return std::make_pair("sample05_0bb3646f15e8dc61f525f40f2884de57.ogg", "out_caller_sample05_0bb3646f15e8dc61f525f40f2884de57.ogg");  // bad

    }
#endif

    if (argc > 2) {
        return std::make_pair(std::string{argv[1]}, std::string{argv[2]});
    } else {
        return std::make_pair("", "");
    }
}

std::vector<double_t> calcBandsPower(const std::vector<int16_t>& signal, std::vector<double_t>& realBuf, std::vector<double_t>& imagBuf) {
    const size_t len{signal.size()};
    assert(len == (size_t)PowOf2(FloorLog2(len)));  // len should be a power of 2
    const double_t mean{0.0};  // assume where's no DC
    const size_t spec_len{len / 2};  // spectrum amplitudes are symmetrical - will use only first half

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

    // df = 1 / (dt * PointsCount) = RegFreq / PointsCount
    const double_t df{RegFreq / len};

    // 24000 (= RegFreq / 2) / 100 ~ 240 bands
    const size_t pointsInBand{FrequencyToIndex(BandWidth, df)};

    for (size_t i = 0; i < spec_len; i += pointsInBand) {
        // limit last band to spectrum highest freq
        size_t bandLen;
        if ((i + pointsInBand) > spec_len) {
            bandLen = spec_len - i;
        } else {
            bandLen = pointsInBand;
        }

        const double_t rms = RmsOnSpectrum(&realBuf[i], bandLen);
        res.emplace_back(rms);
    }

    return res;
}

double_t variance(const std::vector<double_t>& v) {
    assert(!v.empty());

    const double_t mean = Mean_RateFloat(v.data(), v.size());

    double_t sqrSum{0.0};
    for (double_t i : v) {
        sqrSum += pow(i - mean, 2);
    }
    return sqrt(sqrSum / v.size());
}

double_t calcRate(double_t variance) {
    // 5 - (var - 0.7), depends on magic in calcBandsPower
    return fmax(5.0 - fmax(variance - 0.7, 0.0), 1.0);
}
