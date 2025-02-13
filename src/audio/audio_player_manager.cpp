#include "audio_player_manager.hpp"
#include "../global_module.hpp"
#include "../interfaces/audio.hpp"
#include <brightcpp/audio.hpp>
#include <iostream>

BRIGHTCPP_START

namespace internal {

audio_player_manager::audio_player_manager() {
    // ensure audio thread running
    global_module<audio_backend>::get();
}

handle_holder<audio_player>::handle_value_t audio_player_manager::register_player() {
    auto new_id = player_serial_count;
    player_serial_count++;
    players[new_id] = {};
    return new_id;
}

std::unique_ptr<audio_player_backend> &audio_player_manager::get_player(handle_holder<audio_player>::handle_value_t id) {
    return players.at(id);
}

} // namespace internal

BRIGHTCPP_END
