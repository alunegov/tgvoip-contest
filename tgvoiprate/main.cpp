#include <iostream>

#include <opusfile-reader.h>

std::pair<std::string, std::string> parseArgs(int argc, const char** argv);

int main(int argc, const char** argv) {
    try {
        const auto conf = parseArgs(argc, argv);

        OpusFileReader refR;
        OpusFileReader testR;

        refR.Open(conf.first);
        testR.Open(conf.second);
    } catch (std::exception& e) {
        std::cerr << e.what();
        return -13;
    }
}

std::pair<std::string, std::string> parseArgs(int argc, const char** argv) {
    if (argc == 1) {
        std::cout << "Usage: tgvoiprate /path/to/sound_A.opus /path/to/sound_output_A.opus" << std::endl;
        std::cout << "Will use defaults for now" << std::endl;

        return std::make_pair("ref.ogg", "test.ogg");
    }

    return std::make_pair(std::string{argv[1]}, std::string{argv[2]});
}