#include <reader.h>

#include <iostream>

#include <wavefile-writer.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " in.ogg out.wav";
        return -1;
    }

    OpusFileReader r;
    WaveFileWriter w;

    if (!r.Open(argv[1])) {
        std::cout << "in open error" << std::endl;
        return -2;
    }

    if (!w.Create(argv[2])) {
        std::cout << "out create error" << std::endl;
        return -3;
    }

    const auto MaxLen{960};
    int16_t buf[MaxLen];
    size_t j{0};
    while (r.Read(buf, MaxLen)) {
        // to csv via stdout
        /*for (size_t i = 0; i < MaxLen; i++) {
            std::cout << j + i << ";" << buf[i] << std::endl;
        }*/

        // to wave
        if (!w.Write(buf, MaxLen)) {
            std::cout << "out write error at " << j << " byte" << std::endl;
        }

        j += MaxLen;
    }

    if (!w.Commit()) {
        std::cout << "out commit error" << std::endl;
        return -4;
    }
}
