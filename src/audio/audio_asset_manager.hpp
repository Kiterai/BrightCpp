#pragma once

#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

BRIGHTCPP_START

namespace internal {

struct audio_buffer_info {
    const float *begin, *end;
};

class audio_asset_manager {
    size_t serial_id = 0;
    std::unordered_map<size_t, std::vector<float>> loaded_audios;

  public:
    ~audio_asset_manager() = default;

    handle_holder<audio>::handle_value_t make(std::filesystem::path path, audio_file_type type);
    void destroy(handle_holder<audio>::handle_value_t) noexcept;

    audio_buffer_info get_info(handle_holder<audio>::handle_value_t) const;
};

} // namespace internal

BRIGHTCPP_END
