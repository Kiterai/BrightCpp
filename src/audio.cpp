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

handle_holder<audio_player>::handle_value_t player_serial_count = 1;
std::unordered_map<handle_holder<audio_player>::handle_value_t, std::unique_ptr<internal::audio_player_backend>> players;

handle_holder<audio_player>::handle_value_t register_player() {
    // ensure audio thread running
    internal::global_module<internal::audio_backend>::get();

    auto new_id = player_serial_count;
    player_serial_count++;
    players[new_id] = {};
    return new_id;
}

audio::audio(const char *path, audio_file_type type) : handle_holder{g_audio_asset_manager::get().make(path, type)} {}

audio_player::audio_player() : handle_holder{register_player()} {}

audio_player::audio_player(audio &data) : handle_holder{register_player()} {
    set(data);
}

audio_player::audio_player(streaming_audio &data) : handle_holder{register_player()} {
    set(data);
}

void audio_player::set(audio &data) {
    players.at(handle()) = std::make_unique<internal::audio_player_impl_normal>(data);
}

void audio_player::set(streaming_audio &data) {
    // TODO
}

void audio_player::reset() {
    players.at(handle()).reset();
}

audio_player &audio_player::play_once() {
    players.at(handle())->play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    players.at(handle())->play_loop(loop_point);
    return *this;
}

void audio_player::pause() {
    players.at(handle())->pause();
}

void audio_player::resume() {
    players.at(handle())->resume();
}

void audio_player::stop() {
    players.at(handle())->stop();
}

void audio_player::seek(std::chrono::nanoseconds point) {
    players.at(handle())->seek(point);
}

std::chrono::nanoseconds audio_player::pos() const {
    return players.at(handle())->pos();
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
