#include "ogg.hpp"
#include <stdexcept>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

BRIGHTCPP_START

namespace internal {

class oggvorbis_loader : public audio_loader_backend {
  public:
    std::vector<float> load_full_from_file(std::filesystem::path path) {
        FILE *fp = std::fopen(path.string().c_str(), "rb");

        OggVorbis_File vf;
        if (ov_open(fp, &vf, NULL, 0) < 0)
            throw std::runtime_error("failed to ov_open");

        auto vi = ov_info(&vf, -1);
        if (vi == nullptr)
            throw std::runtime_error("failed to ov_info");

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

        ov_clear(&vf);
        fclose(fp);

        return loaded_audio;
    }
};

std::unique_ptr<audio_loader_backend> make_oggvorbis_loader() {
    return std::make_unique<oggvorbis_loader>();
}

} // namespace internal

BRIGHTCPP_END
