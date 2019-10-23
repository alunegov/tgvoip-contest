#pragma once

#include <atomic>
#include <cstdint>
#include <string>

class OpusFileReader {
public:
    OpusFileReader() = default;
    ~OpusFileReader();
    bool Open(const std::string& fileName);
    bool Read(int16_t* data, size_t len);
    bool Seek(int64_t pos);
    bool Eof() const;
private:
    void* file{nullptr};
    int old_li{-1};
    std::atomic_bool eof{false};
};
