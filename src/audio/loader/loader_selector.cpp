#include "loader_selector.hpp"
#include "mp3.hpp"
#include "ogg.hpp"
#include "wav.hpp"

BRIGHTCPP_START

namespace internal {

std::unique_ptr<audio_loader_backend> make_loader(std::filesystem::path path, audio_file_type type) {
    if (type == audio_file_type::auto_detect) {
        if (path.extension() == ".wav")
            type = audio_file_type::wav;
        else if (path.extension() == ".ogg")
            type = audio_file_type::ogg;
        else if (path.extension() == ".mp3")
            type = audio_file_type::mp3;
        else
            throw std::runtime_error("unknown audio file extension: " + path.extension().string());
    }

    switch (type) {
    case audio_file_type::wav:
        return make_wavriff_loader();
        break;
    case audio_file_type::ogg:
        return make_oggvorbis_loader();
        break;
    case audio_file_type::mp3:
        return make_mp3_loader();
        break;
    case audio_file_type::flac:
        throw std::runtime_error("not implemented");
        break;
    default:
        throw std::runtime_error("invalid audio file type");
        break;
    }
}

} // namespace internal

BRIGHTCPP_END
