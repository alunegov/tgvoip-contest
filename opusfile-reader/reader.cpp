#include <reader.h>

#include <cassert>
#include <cstring>

#include <opusfile.h>

OpusFileReader::OpusFileReader() {
}

OpusFileReader::~OpusFileReader() {
    if (file != nullptr) {
        op_free((OggOpusFile *) file);
    }
}

bool OpusFileReader::Open(const std::string& fileName) {
    assert(file == nullptr);

    file = op_open_file(fileName.c_str(), nullptr);
    if (file == nullptr) {
        return false;
    }

    auto head = op_head((const OggOpusFile*)file, 0);
    old_li = -1;
    eof = false;

    return true;
}

bool OpusFileReader::Read(int16_t* data, size_t len) {
    assert(file != nullptr);
    assert(data != nullptr);

    if (eof) {
        memset(data, 0, len * sizeof(int16_t));
        return false;
    }

    size_t all_read = 0;
    while (all_read < len) {
        int li;
        const int nb_read = op_read((OggOpusFile*)file, &data[all_read], len - all_read, &li);
        if (nb_read < 0) {
            if (nb_read == OP_HOLE) {
                // hole in data
                continue;
            } else {
                // decoding error
                break;
            }
        }
        if (nb_read == 0) {
            // decoding complete
            break;
        }

        all_read += nb_read;

        if (li != old_li) {
            auto head = op_head((const OggOpusFile*)file, li);
        }
        old_li = li;
    }

    if (all_read < len) {
        memset(&data[all_read], 0, (len - all_read) * sizeof(int16_t));
    }

    eof = all_read == 0;

    return !eof;
}

bool OpusFileReader::Eof() const {
    return eof;
}
