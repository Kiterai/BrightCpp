#include "wav.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

struct wav_header {
    uint32_t chunk_id;   // "RIFF"
    uint32_t chunk_size; // 4 + (8 + sub_chunk_fmt_size(16)) + (8 + sub_chunk_data_size)
    uint32_t form_type;  // "WAVE"
};

struct subchunk_header {
    uint32_t subchunk_id;
    uint32_t length;
};

struct fmt_chunk {
    uint16_t format_tag; // 1:LPCM
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec; // samples_per_sec * channels * bits_per_sample / 8
    uint16_t block_align;       // channels * bits_per_sample / 8
    uint16_t bits_per_sample;   // 8 or 16
};

class wavriff_loader : public audio_loader_backend {
    std::ifstream f;
    fmt_chunk format_info;
    size_t data_chunk_length, p = 0;
    std::streampos data_chunk_pos;

  public:
    audio_loaded_meta open(std::filesystem::path path) override {
        f.open(path, std::ios::binary);

        wav_header header;
        f.read(reinterpret_cast<char *>(&header), sizeof(header));

        // inversed due to little endian
        if (header.chunk_id != 'FFIR')
            throw std::runtime_error("invalid wav file: chunk id != \"RIFF\"");
        if (header.form_type != 'EVAW')
            throw std::runtime_error("invalid wav file: form type != \"WAVE\"");

        bool fmt_chunk_exists = false, data_chunk_exists = false;

        while (!f.eof()) {
            subchunk_header chunkheader;
            f.read(reinterpret_cast<char *>(&chunkheader), sizeof(chunkheader));

            if (chunkheader.subchunk_id == ' tmf') {
                if (chunkheader.length < 16)
                    throw std::runtime_error("invalid wav file: fmt chunk size < 16");

                std::vector<char> buf(chunkheader.length);
                f.read(buf.data(), chunkheader.length);

                format_info = *reinterpret_cast<const fmt_chunk *>(buf.data());

                if (format_info.format_tag != 1)
                    throw std::runtime_error("invalid wav file: format is not Linear PCM");
                if (format_info.bits_per_sample != 8 && format_info.bits_per_sample != 16)
                    throw std::runtime_error("invalid wav file: bits per sample is not 8 or 16");

                fmt_chunk_exists = true;
            } else if (chunkheader.subchunk_id == 'atad') {
                if (!fmt_chunk_exists)
                    throw std::runtime_error("invalid wav file: data chunk before fmt chunk");
                data_chunk_length = chunkheader.length;
                data_chunk_pos = f.tellg();

                data_chunk_exists = true;
                break;
            } else {
                f.seekg(chunkheader.length, std::ios_base::cur);
            }
        }

        if (!data_chunk_exists)
            throw std::runtime_error("invalid wav file: data chunk not exists");

        return audio_loaded_meta{
            .samplerate = float(format_info.samples_per_sec),
        };
    }
    std::vector<float> load_full() override {
        std::vector<float> loaded_audio;

        for (; p < data_chunk_length;) {
            char buf[16];
            f.read(buf, format_info.block_align);
            p += format_info.block_align;

            if (format_info.bits_per_sample == 8) {
                loaded_audio.push_back((float(*reinterpret_cast<uint8_t *>(buf)) - 128) / 128.0f);
            } else if (format_info.bits_per_sample == 16) {
                loaded_audio.push_back(float(*reinterpret_cast<int16_t *>(buf)) / 32768.0f);
            }
        }

        return loaded_audio;
    }
    size_t load_chunk(float *out, size_t max_sample) override {
        size_t loaded = 0;
        for (; p < data_chunk_length && max_sample > 0;) {
            char buf[16];
            f.read(buf, format_info.block_align);
            p += format_info.block_align;

            if (format_info.bits_per_sample == 8) {
                *out = (float(*reinterpret_cast<uint8_t *>(buf)) - 128) / 128.0f;
            } else if (format_info.bits_per_sample == 16) {
                *out = float(*reinterpret_cast<int16_t *>(buf)) / 32768.0f;
            }
            out++;
            loaded++;
            max_sample--;
        }

        return loaded;
    }
    void seek(size_t frame) {
        auto pos = data_chunk_pos;
        p = frame * format_info.block_align;
        pos += p;
        f.seekg(pos, std::ios_base::beg);
    }
};

std::unique_ptr<audio_loader_backend> make_wavriff_loader() {
    return std::make_unique<wavriff_loader>();
}

} // namespace internal

BRIGHTCPP_END
