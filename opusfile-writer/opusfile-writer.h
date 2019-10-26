#pragma once

#include <cstdint>
#include <string>

class OpusFileWriter {
public:
    OpusFileWriter() = default;
    ~OpusFileWriter();
    bool Create(const std::string& fileName);
    bool Write(int16_t* data, size_t len);
    bool Commit();
private:
    void* enc{nullptr};
    bool isEmpty{true};
    bool isCommited{false};
};
