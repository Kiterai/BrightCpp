#include "streaming_audio_asset_manager.hpp"
#include "loader/loader_selector.hpp"
#include "resample.hpp"

BRIGHTCPP_START

namespace internal {

entity_handle_t streaming_audio_asset_manager::make(std::filesystem::path path, audio_file_type type) {
    auto loader = make_loader(path, type);
    loader->open(path);
    return streaming_audios.make(std::move(loader));
}

void streaming_audio_asset_manager::destroy(entity_handle_t handle) noexcept { streaming_audios.destroy(handle); }

audio_loader_backend &streaming_audio_asset_manager::get_loader(entity_handle_t handle) const {
    return *streaming_audios.get(handle).get();
}

} // namespace internal

BRIGHTCPP_END
