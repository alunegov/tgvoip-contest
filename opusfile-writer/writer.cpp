#include <writer.h>

#include <cassert>

#include <opusenc.h>

OpusFileWriter::OpusFileWriter() {
}

OpusFileWriter::~OpusFileWriter() {
    ope_encoder_destroy((OggOpusEnc*)encData.enc);
}

bool OpusFileWriter::Create(const std::string& fileName) {
    assert(encData.enc == nullptr);

    const auto rate{48000};
    const auto chan{1};

    OpusEncCallbacks callbacks{
        write_callback,
        close_callback
    };

    encData.enc = ope_encoder_create_callbacks(&callbacks, &encData, nullptr, rate, chan, (chan > 8) ? 255 : chan > 2, &ret);
    if (encData.enc == nullptr) {
        return false;
    }

    return true;
}

bool OpusFileWriter::Write(int16_t* data, size_t len) {
    assert(encData.enc != nullptr);

    ret = ope_encoder_write((OggOpusEnc*)encData.enc, data, len);

    return ret == OPE_OK;
}

bool OpusFileWriter::Flush() {
    assert(encData.enc != nullptr);

    if (ret == OPE_OK) ret = ope_encoder_drain((OggOpusEnc*)encData.enc);

    return ret == OPE_OK;
}