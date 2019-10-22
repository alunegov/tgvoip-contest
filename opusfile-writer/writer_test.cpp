#include <writer.h>

#include <iostream>

#include <wavefile-reader.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " in.wav out.ogg";
        return -1;
    }

    WaveFileReader r;
    OpusFileWriter w;

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
        // to ogg
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
