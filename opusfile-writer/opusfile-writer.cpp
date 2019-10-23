#include <opusfile-writer.h>

#include <cassert>

#include <opusenc.h>

OpusFileWriter::~OpusFileWriter() {
    if (enc != nullptr) {
        Commit();
        ope_encoder_destroy((OggOpusEnc *)enc);
    }
}

bool OpusFileWriter::Create(const std::string& fileName) {
    assert(enc == nullptr);
    assert(!isCommited);

    const auto rate{48000};
    const auto chan{1};
    const auto family{(chan > 8) ? 255 : chan > 2};

    OggOpusComments* comments = ope_comments_create();

    enc = ope_encoder_create_file(fileName.c_str(), comments, rate, chan, family, nullptr);

    ope_comments_destroy(comments);

    if (enc == nullptr) {
        return false;
    }

    auto ret = ope_encoder_ctl((OggOpusEnc*)enc, OPUS_SET_BITRATE(64000));
    if (ret != OPE_OK) {
        return false;
    }

    return true;
}

bool OpusFileWriter::Write(int16_t* data, size_t len) {
    assert(enc != nullptr);
    assert(!isCommited);

    auto ret = ope_encoder_write((OggOpusEnc*)enc, data, len);
    if (ret != OPE_OK) {
        return false;
    }

    return true;
}

bool OpusFileWriter::Commit() {
    assert(enc != nullptr);

    if (isCommited) {
        return true;
    }

    auto ret = ope_encoder_drain((OggOpusEnc*)enc);
    if (ret != OPE_OK) {
        return false;
    }

    isCommited = true;

    return true;
}
