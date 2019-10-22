#include <wavefile-writer.h>

#include <cassert>
#include <iostream>

WaveFileWriter::WaveFileWriter() {
}

WaveFileWriter::~WaveFileWriter() {
    if (file != nullptr) {
        Commit();
        fclose(file);
    }
}

bool WaveFileWriter::Create(const std::string& fileName) {
    assert(file == nullptr);
    assert(dataBytesWritten == 0);
    assert(!isCommited);

    file = fopen(fileName.c_str(), "wb");
    if (file == nullptr) {
        return false;
    }

    auto ret = fseek(file, sizeof(WaveHeader), SEEK_SET);
    if (ret != 0) {
        return false;
    }

    return true;
}

bool WaveFileWriter::Write(int16_t* data, size_t len) {
    assert(file != nullptr);
    assert(!isCommited);
    assert(data != nullptr);

    auto ret = fwrite(data, sizeof(int16_t), len, file);

    dataBytesWritten += sizeof(int16_t) * ret;

    if (ret != len) {
        return false;
    }

    return true;
}

bool WaveFileWriter::Commit() {
    assert(file != nullptr);

    if (isCommited) {
        return true;
    }

    auto ret = fseek(file, 0, SEEK_SET);
    if (ret != 0) {
        return false;
    }

    WaveHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.Marker1[0] = 'R';hdr.Marker1[1] = 'I';hdr.Marker1[2] = 'F';hdr.Marker1[3] = 'F';
    hdr.BytesFollowing = dataBytesWritten + 36;
    hdr.Marker2[0] = 'W';hdr.Marker2[1] = 'A';hdr.Marker2[2] = 'V';hdr.Marker2[3] = 'E';
    hdr.Marker3[0] = 'f';hdr.Marker3[1] = 'm';hdr.Marker3[2] = 't';hdr.Marker3[3] = ' ';
    hdr.Fixed1 = 16;
    hdr.FormatTag = 1;
    hdr.Channels = 1;
    hdr.SampleRate = 48000;
    hdr.BytesPerSecond = 96000;
    hdr.BytesPerSample = 2;
    hdr.BitsPerSample = 16;
    hdr.Marker4[0] = 'd';hdr.Marker4[1] = 'a';hdr.Marker4[2] = 't';hdr.Marker4[3] = 'a';
    hdr.DataBytes = dataBytesWritten;
    ret = fwrite(&hdr, sizeof(hdr), 1, file);
    if (ret != 1) {
        return false;
    }

    ret = fflush(file);
    if (ret != 0) {
        return false;
    }

    isCommited = true;

    return true;
}