#pragma once

#include "../global_module.hpp"
#include "loader/loader.hpp"
#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <cstdint>
#include <filesystem>
#include <unordered_map>

BRIGHTCPP_START

namespace internal {

class streaming_audio_manager {
    size_t serial_id = 0;
    std::unordered_map<size_t, std::unique_ptr<audio_loader_backend>> streaming_audios;

  public:
    ~streaming_audio_manager() = default;

    handle_holder<streaming_audio>::handle_value_t make(std::filesystem::path path, audio_file_type type);
    void destroy(handle_holder<streaming_audio>::handle_value_t) noexcept;

    audio_loader_backend &get_loader(handle_holder<streaming_audio>::handle_value_t) const;
};

} // namespace internal

BRIGHTCPP_END
