#include "streaming_audio_manager.hpp"
#include "loader/loader_selector.hpp"
#include "resample.hpp"

BRIGHTCPP_START

namespace internal {

handle_holder<audio>::handle_value_t streaming_audio_manager::make(std::filesystem::path path, audio_file_type type) {
    streaming_audios[serial_id] = make_loader(path, type);
    auto meta = streaming_audios[serial_id]->open(path);
    auto id = serial_id;
    serial_id++;
    return id;
}

void streaming_audio_manager::destroy(handle_holder<audio>::handle_value_t handle) noexcept {
    streaming_audios.erase(handle);
}

audio_loader_backend &streaming_audio_manager::get_loader(handle_holder<audio>::handle_value_t handle) const {
    return *streaming_audios.at(handle).get();
}

} // namespace internal

BRIGHTCPP_END
