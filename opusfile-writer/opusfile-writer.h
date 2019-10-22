#pragma once

#include <cstdint>
#include <string>

struct EncData {
    void* enc{nullptr};
};

class OpusFileWriter {
public:
    OpusFileWriter();
    ~OpusFileWriter();
    bool Create(const std::string& fileName);
    bool Write(int16_t* data, size_t len);
    bool Commit();
private:
    EncData encData;
    bool isCommited{true};
};