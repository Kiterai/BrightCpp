#include "ogg.hpp"
#include <iostream>
#include <stdexcept>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

BRIGHTCPP_START

namespace internal {

class oggvorbis_loader : public audio_loader_backend {
    FILE *fp = nullptr;
    OggVorbis_File vf;

  public:
    ~oggvorbis_loader() override {
        std::cout << "b" << std::endl;
        ov_clear(&vf);
        if (fp)
            fclose(fp);
    }
    audio_loaded_meta open(std::filesystem::path path) override {
        std::cout << "a" << std::endl;
        fp = std::fopen(path.string().c_str(), "rb");

        if (ov_open(fp, &vf, NULL, 0) < 0)
            throw std::runtime_error("failed to ov_open");

        auto vi = ov_info(&vf, -1);
        if (vi == nullptr)
            throw std::runtime_error("failed to ov_info");
        auto samplerate = vi->rate;

        return audio_loaded_meta{
            .samplerate = float(samplerate),
        };
    }
    std::vector<float> load_full() override {
        std::vector<float> loaded_audio;
        constexpr size_t bufsize = 4096;
        std::vector<char> buffer(bufsize);
        auto p = reinterpret_cast<int16_t *>(buffer.data());
        int cur, decoded;

        do {
            decoded = ov_read(&vf, buffer.data(), bufsize, 0, 2, 1, &cur);
            for (int i = 0; i < decoded / (2 * sizeof(int16_t)); i++)
                loaded_audio.push_back(float(p[i * 2]) / 32768);
        } while (decoded > 0);

        return loaded_audio;
    }
    size_t load_chunk(float *out, size_t max_sample) override {
        constexpr size_t bufsize = 4096;
        std::vector<char> buffer(bufsize);
        auto p = reinterpret_cast<int16_t *>(buffer.data());
        int cur, decoded;
        size_t decoded_sum = 0;

        int block_size = 2 * sizeof(int16_t);
        do {
            decoded = ov_read(&vf, buffer.data(), std::min(bufsize, (max_sample - decoded_sum) * block_size), 0, 2, 1, &cur);
            for (int i = 0; i < decoded / block_size; i++) {
                *out = float(p[i * 2]) / 32768;
                out++;
                decoded_sum++;
            }
        } while (decoded > 0 && decoded_sum < max_sample);

        return decoded_sum;
    }
};

std::unique_ptr<audio_loader_backend> make_oggvorbis_loader() {
    return std::make_unique<oggvorbis_loader>();
}

} // namespace internal

BRIGHTCPP_END
