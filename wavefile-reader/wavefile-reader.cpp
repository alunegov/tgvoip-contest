#include <wavefile-reader.h>

#include <cassert>
#include <cstring>

WaveFileReader::WaveFileReader() {
}

WaveFileReader::~WaveFileReader() {
    if (file != nullptr) {
        fclose(file);
    }
}

bool WaveFileReader::Open(const std::string& fileName) {
    assert(file == nullptr);

    file = fopen(fileName.c_str(), "rb");
    if (file == nullptr) {
        return false;
    }

    WaveHeader hdr;
    auto ret = fread(&hdr, sizeof(hdr), 1, file);
    if (ret != 1) {
        return false;
    }

    return true;
}

bool WaveFileReader::Read(int16_t* data, size_t len) {
    assert(file != nullptr);
    assert(data != nullptr);

    const size_t n_read = fread(data, sizeof(int16_t), len, file);

    if (n_read < len) {
        memset(&data[n_read], 0, (len - n_read) * sizeof(int16_t));
    }

    return n_read != 0;
}
