#pragma once

#include "../entity_holder.hpp"
#include "player/player.hpp"
#include <filesystem>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_player_manager {
    entity_holder<std::unique_ptr<audio_player_backend>> db;

  public:
    audio_player_manager();
    entity_handle_t register_player();
    std::unique_ptr<audio_player_backend> &get_player(entity_handle_t id);
};

} // namespace internal

BRIGHTCPP_END
