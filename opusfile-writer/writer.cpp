#include <writer.h>

#include <cassert>

#include <opusenc.h>

OpusFileWriter::OpusFileWriter() {
}

OpusFileWriter::~OpusFileWriter() {
    if (encData.enc != nullptr) {
        ope_encoder_destroy((OggOpusEnc *) encData.enc);
    }
}

bool OpusFileWriter::Create(const std::string& fileName) {
    assert(encData.enc == nullptr);

    const auto rate{48000};
    const auto chan{1};

    OggOpusComments* comments = ope_comments_create();
    ope_comments_add(comments, "a", "b");

    encData.enc = ope_encoder_create_file(fileName.c_str(), comments, rate, chan, (chan > 8) ? 255 : chan > 2, &ret);
    ope_comments_destroy(comments);
    if (encData.enc == nullptr) {
        return false;
    }

    ret = ope_encoder_ctl((OggOpusEnc*)encData.enc, OPUS_SET_BITRATE(64000));

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