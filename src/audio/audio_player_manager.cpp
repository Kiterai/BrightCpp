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

entity_handle_t audio_player_manager::register_player() {
    std::unique_ptr<audio_player_backend> tmp{};
    return db.make(std::move(tmp));
}

std::unique_ptr<audio_player_backend> &audio_player_manager::get_player(entity_handle_t id) { return db.get(id); }

} // namespace internal

BRIGHTCPP_END
