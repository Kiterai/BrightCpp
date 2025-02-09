#include "audio_asset_manager.hpp"
#include "loader/loader_selector.hpp"
#include "resample.hpp"

BRIGHTCPP_START

namespace internal {

handle_holder<audio>::handle_value_t audio_asset_manager::make(std::filesystem::path path, audio_file_type type) {
    auto loader = make_loader(path, type);
    auto buf = loader->load_full_from_file(path);

    auto buf_resampled = instant_full_resample(buf, 44100, 48000);

    loaded_audios[serial_id] = std::move(buf_resampled);
    auto id = serial_id;
    serial_id++;
    return id;
}

void audio_asset_manager::destroy(handle_holder<audio>::handle_value_t handle) noexcept {
    loaded_audios.erase(handle);
}

audio_buffer_info audio_asset_manager::get_info(handle_holder<audio>::handle_value_t handle) const {
    const auto &entry = loaded_audios.at(handle);
    return audio_buffer_info{
        .begin = entry.data(),
        .end = entry.data() + entry.size(),
    };
}

} // namespace internal

BRIGHTCPP_END
