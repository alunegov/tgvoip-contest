#pragma once

#include <cstdint>
#include <string>

class WaveFileWriter {
public:
    WaveFileWriter();
    ~WaveFileWriter();
    bool Create(const std::string& fileName);
    bool Write(int16_t* data, size_t len);
    bool Commit();
private:
    FILE* file{nullptr};
    int dataBytesWritten{0};
    bool isCommited{true};
};

struct WaveHeader {
    uint8_t Marker1[4];
    int32_t BytesFollowing;
    uint8_t Marker2[4];
    uint8_t Marker3[4];
    int32_t Fixed1;
    uint16_t FormatTag;
    uint16_t Channels;
    int32_t SampleRate;
    int32_t BytesPerSecond;
    uint16_t BytesPerSample;
    uint16_t BitsPerSample;
    uint8_t Marker4[4];
    int32_t DataBytes;
};
