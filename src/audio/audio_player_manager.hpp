#pragma once

#include "player/player.hpp"
#include <brightcpp/audio.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_player_manager {
    handle_holder<audio_player>::handle_value_t player_serial_count = 1;
    std::unordered_map<handle_holder<audio_player>::handle_value_t, std::unique_ptr<audio_player_backend>> players;

  public:
    audio_player_manager();
    handle_holder<audio_player>::handle_value_t register_player();
    std::unique_ptr<audio_player_backend> &get_player(handle_holder<audio_player>::handle_value_t id);
};

} // namespace internal

BRIGHTCPP_END
