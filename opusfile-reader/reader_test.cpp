#include <reader.h>

#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " file.ogg";
        return 0;
    }

    OpusFileReader r;

    r.Open(argv[1]);

    const auto MaxLen{960};
    int16_t buf[MaxLen];
    size_t j{1};
    while (r.Read(buf, MaxLen)) {
        //std::cout << "got " << MaxLen << " bytes, buf " << buf[0] << " " << buf[1] << " " <<  buf[2] << "..."
        //    << buf[MaxLen - 3] << " " << buf[MaxLen - 2] << " " <<  buf[MaxLen - 1] << std::endl;
        for (size_t i = 0; i < MaxLen; i++) {
            std::cout << j + i << "," << buf[i] << std::endl;
        }
        j += MaxLen;
    }
}
