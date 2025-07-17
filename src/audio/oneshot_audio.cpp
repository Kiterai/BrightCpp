#include "audio_asset_manager.hpp"
#include "mixer.hpp"
#include "../global_module.hpp"
#include "../interfaces/audio.hpp"
#include <brightcpp/audio.hpp>
#include <stdexcept>

BRIGHTCPP_START

using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;
using g_audio_mixer = internal::global_module<internal::audio_mixer>;

oneshot_audio_player::oneshot_audio_player() {
    // ensure audio thread running
    internal::global_module<internal::audio_backend>::get();
}

void oneshot_audio_player::play_oneshot(audio &data) {
    const auto info = g_audio_asset_manager::get().get_info(data.handle());

    g_audio_mixer::get().add_playing(internal::audio_play_info{
        .delay_timer = 0,
        .current_pos = info.begin,
        .end_pos = info.end,
        .volume = 1.0,
        .mode = internal::audio_play_info::play_mode::oneshot,
        .stopped = false,
        .paused = false,
    });
}

// TODO
void oneshot_audio_player::pause() {
    throw std::runtime_error("not implemented");
}

void oneshot_audio_player::resume() {
    throw std::runtime_error("not implemented");
}

void oneshot_audio_player::stop() {
    throw std::runtime_error("not implemented");
}

bool oneshot_audio_player::busy_any() const {
    throw std::runtime_error("not implemented");
}

BRIGHTCPP_END
