#pragma once

#include "../entity_holder.hpp"
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
    entity_holder<std::vector<float>> db;

  public:
    ~audio_asset_manager() = default;

    entity_handle_t make(std::filesystem::path path, audio_file_type type);
    void destroy(entity_handle_t) noexcept;

    audio_buffer_info get_info(entity_handle_t) const;
};

} // namespace internal

BRIGHTCPP_END
