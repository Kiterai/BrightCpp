#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "mp3.hpp"
#include <minimp3_ex.h>
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

class mp3_loader : public audio_loader_backend {
    mp3dec_t mp3d;
    mp3dec_file_info_t info;

  public:
    ~mp3_loader() override {
        free(info.buffer);
    }
    audio_loaded_result load_full_from_file2(std::filesystem::path path) override {
        std::vector<float> loaded_audio;

        mp3dec_t mp3d;
        mp3dec_file_info_t info;
        if (mp3dec_load(&mp3d, path.string().c_str(), &info, NULL, NULL)) {
            throw std::runtime_error("failed to decode mp3");
        }

        for (int i = 0; i < info.samples; i += info.channels) {
            loaded_audio.push_back(info.buffer[i]);
        }

        free(info.buffer);

        return audio_loaded_result{
            .buf = std::move(loaded_audio),
            .samplerate = float(info.hz),
        };
    }
    audio_loaded_meta open(std::filesystem::path path) override {
        if (mp3dec_load(&mp3d, path.string().c_str(), &info, NULL, NULL)) {
            throw std::runtime_error("failed to decode mp3");
        }

        return audio_loaded_meta{
            .samplerate = float(info.hz),
        };
    }
    std::vector<float> load_full() override {
        std::vector<float> loaded_audio;
        for (int i = 0; i < info.samples; i += info.channels) {
            loaded_audio.push_back(info.buffer[i]);
        }

        return loaded_audio;
    }
    // size_t load_chunk(float *out, size_t max_sample) override {
    // }
};

std::unique_ptr<audio_loader_backend> make_mp3_loader() {
    return std::make_unique<mp3_loader>();
}

} // namespace internal

BRIGHTCPP_END
