#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include <opusfile-reader.h>
#include <rate-math.h>

std::vector<double_t> calcBandsPower(int16_t* signal, size_t len) {
    assert(len == (size_t)PowOf2(FloorLog2(len)));

    const auto mean{0.0};
    auto real = std::make_unique<double_t[]>(len);
    auto imag = std::make_unique<double_t[]>(len);
    const auto spec_len{len / 2};

    Rfft(signal, real.get(), imag.get(), len, mean, false);

    Cmplx_Ampl(real.get(), imag.get(), len, spec_len);

    // df = 1 / (dt * PointsCount) = RegFreq / PointsCount
    const auto df{48000.0 / len};
    const auto pointsInBand{FrequencyToIndex(100, df)};

    std::vector<double_t> res;

    for (size_t i = 0; i < spec_len; i += pointsInBand) {
        const auto rms = Rms_RateFloat(&real.get()[i], pointsInBand, 1, 0, mean);
        res.emplace_back(rms);
    }

    return res;
}

std::pair<std::string, std::string> parseArgs(int argc, const char** argv);

int main(int argc, const char** argv) {
    try {
        const auto conf = parseArgs(argc, argv);

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

        const auto BufLen{16384};
        int16_t refBuf[BufLen];
        int16_t testBuf[BufLen];
        while (refR.Read(refBuf, BufLen) && testR.Read(testBuf, BufLen)) {
            const auto refBandsPower = calcBandsPower(refBuf, BufLen);
            const auto testBandsPower = calcBandsPower(testBuf, BufLen);
        }

        // TODO: call dtors of refR, testR to catch their errors?

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what();
        return -13;
    }
}

std::pair<std::string, std::string> parseArgs(int argc, const char** argv) {
    if (argc == 1) {
        std::cout << "Usage: tgvoiprate /path/to/sound_A.opus /path/to/sound_output_A.opus" << std::endl;
        std::cout << "Will use defaults for now" << std::endl;

        return std::make_pair("sample05_0bb3646f15e8dc61f525f40f2884de57.ogg", "out_sample05_0bb3646f15e8dc61f525f40f2884de57.ogg");
    }

    return std::make_pair(std::string{argv[1]}, std::string{argv[2]});
}