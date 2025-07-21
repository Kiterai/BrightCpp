#include "audio_asset_manager.hpp"
#include "loader/loader_selector.hpp"
#include "resample.hpp"

BRIGHTCPP_START

namespace internal {

entity_handle_t audio_asset_manager::make(std::filesystem::path path, audio_file_type type) {
    auto loader = make_loader(path, type);
    auto meta = loader->open(path);
    auto audio_data = loader->load_full();

    return db.make(instant_full_resample(audio_data, meta.samplerate, 48000));
}

void audio_asset_manager::destroy(entity_handle_t handle) noexcept {
    db.destroy(handle);
}

audio_buffer_info audio_asset_manager::get_info(entity_handle_t handle) const {
    const auto &entry = db.get(handle);
    return audio_buffer_info{
        .begin = entry.data(),
        .end = entry.data() + entry.size(),
    };
}

} // namespace internal

BRIGHTCPP_END
