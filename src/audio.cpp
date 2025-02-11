#include "interfaces/audio.hpp"
#include "audio/audio_asset_manager.hpp"
#include "audio/mixer.hpp"
#include "audio/player_normal.hpp"
#include "audio/streaming_manager.hpp"
#include "global_module.hpp"
#include <brightcpp/audio.hpp>
#include <iostream>

BRIGHTCPP_START

using g_audio_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;

audio::audio(const char *path, audio_file_type type) : handle_holder{g_audio_asset_manager::get().make(path, type)} {}

audio_player::audio_player(audio &data) : handle_holder{internal::player_register()} {
    // ensure audio thread running
    internal::global_module<internal::audio_backend>::get();
    internal::players[handle()].set(data);
}

audio_player::audio_player(streaming_audio &data) : handle_holder{internal::player_register()} {
    // internal::players[handle()].set(data);
}

void audio_player::set(audio &data) {
    internal::players[handle()].set(data);
}

void audio_player::reset() {
    internal::players[handle()].reset();
}

audio_player &audio_player::play_once() {
    internal::players[handle()].play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    internal::players[handle()].play_loop(loop_point);
    return *this;
}

void audio_player::pause() {
    internal::players[handle()].pause();
}

void audio_player::resume() {
    internal::players[handle()].resume();
}

void audio_player::stop() {
    internal::players[handle()].stop();
}

void audio_player::seek(std::chrono::nanoseconds point) {
    internal::players[handle()].seek(point);
}

std::chrono::nanoseconds audio_player::pos() const {
    return internal::players[handle()].pos();
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
