#pragma once

#include "../entity_holder.hpp"
#include "../global_module.hpp"
#include "loader/loader.hpp"
#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <cstdint>
#include <filesystem>

BRIGHTCPP_START

namespace internal {

class streaming_audio_asset_manager {
    entity_holder<std::unique_ptr<audio_loader_backend>> streaming_audios;

  public:
    ~streaming_audio_asset_manager() = default;

    entity_handle_t make(std::filesystem::path path, audio_file_type type);
    void destroy(entity_handle_t) noexcept;

    audio_loader_backend &get_loader(entity_handle_t) const;
};

} // namespace internal

BRIGHTCPP_END
