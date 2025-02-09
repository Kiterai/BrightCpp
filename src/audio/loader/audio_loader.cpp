#include "audio_loader.hpp"
#include "ogg.hpp"

BRIGHTCPP_START

namespace internal {

handle_holder<audio>::handle_value_t audio_loader::make(std::filesystem::path path, audio_file_type type) {
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

    std::vector<float> buf;

    switch (type) {
    case audio_file_type::wav:
        throw std::runtime_error("not implemented");
        break;
    case audio_file_type::ogg:
        buf = load_oggvorbis_full(path.string().c_str());
        break;
    case audio_file_type::mp3:
        throw std::runtime_error("not implemented");
        break;
    case audio_file_type::flac:
        throw std::runtime_error("not implemented");
        break;
    default:
        throw std::runtime_error("invalid audio file type");
        break;
    }

    loaded_audios[serial_id] = std::move(buf);
    auto id = serial_id;
    serial_id++;
    return id;
}

void audio_loader::destroy(handle_holder<audio>::handle_value_t handle) noexcept {
    loaded_audios.erase(handle);
}

audio_buffer_info audio_loader::get_info(handle_holder<audio>::handle_value_t handle) const {
    const auto &entry = loaded_audios.at(handle);
    return audio_buffer_info{
        .begin = entry.data(),
        .end = entry.data() + entry.size(),
    };
}

} // namespace internal

BRIGHTCPP_END
