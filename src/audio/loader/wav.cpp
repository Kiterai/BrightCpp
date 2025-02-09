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
  public:
    std::vector<float> load_full_from_file(std::filesystem::path path) override {
        std::vector<float> loaded_audio;

        std::ifstream f{path, std::ios::binary};

        wav_header header;
        f.read(reinterpret_cast<char *>(&header), sizeof(header));

        // inversed due to little endian
        if (header.chunk_id != 'FFIR')
            throw std::runtime_error("invalid wav file: chunk id != \"RIFF\"");
        if (header.form_type != 'EVAW')
            throw std::runtime_error("invalid wav file: form type != \"WAVE\"");

        fmt_chunk format_info;
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

                for (size_t p = 0; p < chunkheader.length;) {
                    char buf[16];
                    f.read(buf, format_info.block_align);
                    p += format_info.block_align;

                    if (format_info.bits_per_sample == 8) {
                        loaded_audio.push_back((float(*reinterpret_cast<uint8_t *>(buf)) - 128) / 128.0f);
                    } else if (format_info.bits_per_sample == 16) {
                        loaded_audio.push_back(float(*reinterpret_cast<int16_t *>(buf)) / 32768.0f);
                    }
                }

                data_chunk_exists = true;
                break;
            } else {
                f.seekg(chunkheader.length, std::ios_base::cur);
            }
        }

        if (!data_chunk_exists)
            throw std::runtime_error("invalid wav file: data chunk not exists");

        return loaded_audio;
    }
    audio_loaded_result load_full_from_file2(std::filesystem::path path) override {
        std::vector<float> loaded_audio;

        std::ifstream f{path, std::ios::binary};

        wav_header header;
        f.read(reinterpret_cast<char *>(&header), sizeof(header));

        // inversed due to little endian
        if (header.chunk_id != 'FFIR')
            throw std::runtime_error("invalid wav file: chunk id != \"RIFF\"");
        if (header.form_type != 'EVAW')
            throw std::runtime_error("invalid wav file: form type != \"WAVE\"");

        fmt_chunk format_info;
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

                for (size_t p = 0; p < chunkheader.length;) {
                    char buf[16];
                    f.read(buf, format_info.block_align);
                    p += format_info.block_align;

                    if (format_info.bits_per_sample == 8) {
                        loaded_audio.push_back((float(*reinterpret_cast<uint8_t *>(buf)) - 128) / 128.0f);
                    } else if (format_info.bits_per_sample == 16) {
                        loaded_audio.push_back(float(*reinterpret_cast<int16_t *>(buf)) / 32768.0f);
                    }
                }

                data_chunk_exists = true;
                break;
            } else {
                f.seekg(chunkheader.length, std::ios_base::cur);
            }
        }

        if (!data_chunk_exists)
            throw std::runtime_error("invalid wav file: data chunk not exists");

        return audio_loaded_result{
            .buf = std::move(loaded_audio),
            .samplerate = float(format_info.samples_per_sec),
        };
    }
};

std::unique_ptr<audio_loader_backend> make_wavriff_loader() {
    return std::make_unique<wavriff_loader>();
}

} // namespace internal

BRIGHTCPP_END
