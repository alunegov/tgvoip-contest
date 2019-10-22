#include <opusfile-writer.h>

#include <cassert>

#include <opusenc.h>

OpusFileWriter::~OpusFileWriter() {
    if (encData.enc != nullptr) {
        Commit();
        ope_encoder_destroy((OggOpusEnc *)encData.enc);
    }
}

bool OpusFileWriter::Create(const std::string& fileName) {
    assert(encData.enc == nullptr);
    assert(!isCommited);

    const auto rate{48000};
    const auto chan{1};

    OggOpusComments* comments = ope_comments_create();

    encData.enc = ope_encoder_create_file(fileName.c_str(), comments, rate, chan, (chan > 8) ? 255 : chan > 2, nullptr);

    ope_comments_destroy(comments);

    if (encData.enc == nullptr) {
        return false;
    }

    auto ret = ope_encoder_ctl((OggOpusEnc*)encData.enc, OPUS_SET_BITRATE(64000));
    if (ret != OPE_OK) {
        return false;
    }

    return true;
}

bool OpusFileWriter::Write(int16_t* data, size_t len) {
    assert(encData.enc != nullptr);
    assert(!isCommited);

    auto ret = ope_encoder_write((OggOpusEnc*)encData.enc, data, len);
    if (ret != OPE_OK) {
        return false;
    }

    return true;
}

bool OpusFileWriter::Commit() {
    assert(encData.enc != nullptr);

    if (isCommited) {
        return true;
    }

    auto ret = ope_encoder_drain((OggOpusEnc*)encData.enc);
    if (ret != OPE_OK) {
        return false;
    }

    isCommited = true;

    return true;
}