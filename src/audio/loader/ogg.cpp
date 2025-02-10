#include "ogg.hpp"
#include <stdexcept>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

BRIGHTCPP_START

namespace internal {

class oggvorbis_loader : public audio_loader_backend {
    FILE *fp;
    OggVorbis_File vf;

  public:
    ~oggvorbis_loader() override {
        ov_clear(&vf);
        fclose(fp);
    }
    audio_loaded_meta open(std::filesystem::path path) override {
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
    // size_t load_chunk(float *out, size_t max_sample) override {
    // }
};

std::unique_ptr<audio_loader_backend> make_oggvorbis_loader() {
    return std::make_unique<oggvorbis_loader>();
}

} // namespace internal

BRIGHTCPP_END
