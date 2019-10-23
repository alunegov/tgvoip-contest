#pragma once

#include <atomic>
#include <cstdint>
#include <string>

// Little-endian only
class WaveFileReader {
public:
    WaveFileReader() = default;
    ~WaveFileReader();
    bool Open(const std::string& fileName);
    bool Read(int16_t* data, size_t len);
    bool Seek(int64_t pos);
private:
    FILE* file{nullptr};
};

struct WaveHeader {
    char Marker1[4];
    int32_t BytesFollowing;
    char Marker2[4];
    char Marker3[4];
    int32_t Fixed1;
    uint16_t FormatTag;
    uint16_t Channels;
    int32_t SampleRate;
    int32_t BytesPerSecond;
    uint16_t BytesPerSample;
    uint16_t BitsPerSample;
    char Marker4[4];
    int32_t DataBytes;
};
