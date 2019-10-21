#pragma once

#include <atomic>
#include <cstdint>
#include <string>

class OpusFileReader {
public:
    OpusFileReader();
    ~OpusFileReader();
    bool Open(const std::string& fileName);
    bool Read(int16_t* data, size_t len);
    bool Eof() const;
private:
    void* file{nullptr};
    int li{0};
    int old_li{-1};
    std::atomic_bool eof{false};
};
