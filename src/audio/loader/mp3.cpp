#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#define NOMINMAX
#include "mp3.hpp"
#include <minimp3_ex.h>
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

class mp3_loader : public audio_loader_backend {
    mp3dec_ex_t dec;
    int ch;

  public:
    ~mp3_loader() override {
        mp3dec_ex_close(&dec);
    }
    audio_loaded_meta open(std::filesystem::path path) override {
        if (mp3dec_ex_open(&dec, path.string().c_str(), MP3D_SEEK_TO_SAMPLE)) {
            throw std::runtime_error("failed to decode mp3");
        }
        ch = dec.info.channels;

        return audio_loaded_meta{
            .samplerate = float(dec.info.hz),
        };
    }
    std::vector<float> load_full() override {
        constexpr size_t decode_samples = 4096;
        std::vector<float> loaded_audio, tmp(decode_samples);
        size_t read;
        do {
            read = mp3dec_ex_read(&dec, tmp.data(), decode_samples);
            for (int i = 0; i < read; i += ch)
                loaded_audio.push_back(tmp[i]);
        } while (read > 0);

        return loaded_audio;
    }
    size_t load_chunk(float *out, size_t max_sample) override {
        constexpr size_t decode_samples = 4096;
        std::vector<float> tmp(decode_samples);
        size_t read_sum = 0, read;

        do {
            read = mp3dec_ex_read(&dec, tmp.data(), std::min(decode_samples, max_sample - read_sum));
            for (int i = 0; i < read; i += ch) {
                *out = tmp[i];
                out++;
                read_sum++;
            }
        } while (read > 0 && read_sum < max_sample);

        return read_sum;
    }
    void seek(size_t frame) {
        mp3dec_ex_seek(&dec, frame);
    }
};

std::unique_ptr<audio_loader_backend> make_mp3_loader() {
    return std::make_unique<mp3_loader>();
}

} // namespace internal

BRIGHTCPP_END
