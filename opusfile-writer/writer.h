#pragma once

#include <cstdint>
#include <string>

class OpusFileWriter {
public:
    OpusFileWriter();
    ~OpusFileWriter();
    bool Create(const std::string& fileName);
    bool Write(int16_t* data, size_t len);
    bool Flush();
private:
    EncData encData;
    int ret{0};
};

struct EncData {
    void* enc{nullptr};
};
