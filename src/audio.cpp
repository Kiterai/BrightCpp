#include "interfaces/audio.hpp"
#include "audio/audio_asset_manager.hpp"
#include "audio/audio_player_manager.hpp"
#include "audio/player/player_normal.hpp"
#include "audio/player/player_streaming.hpp"
#include "audio/streaming_audio_asset_manager.hpp"
#include "audio/streaming_manager.hpp"
#include "global_module.hpp"
#include <brightcpp/audio.hpp>
#include <iostream>

BRIGHTCPP_START

using g_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_streaming_audio_manager = internal::global_module<internal::streaming_audio_asset_manager>;
using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;

using g_audio_player_manager = internal::global_module<internal::audio_player_manager>;

audio::audio(const char *path, audio_file_type type) : handle_holder{g_audio_asset_manager::get().make(path, type)} {}
streaming_audio::streaming_audio(const char *path, audio_file_type type) : handle_holder{g_streaming_audio_manager::get().make(path, type)} {}

audio_player::audio_player() : handle_holder{g_audio_player_manager::get().register_player()} {}

audio_player::audio_player(audio &data) : handle_holder{g_audio_player_manager::get().register_player()} {
    set(data);
}

audio_player::audio_player(streaming_audio &data) : handle_holder{g_audio_player_manager::get().register_player()} {
    set(data);
}

void audio_player::set(audio &data) {
    g_audio_player_manager::get().get_player(handle()) = std::make_unique<internal::audio_player_impl_normal>(data);
}

void audio_player::set(streaming_audio &data) {
    g_audio_player_manager::get().get_player(handle()) = std::make_unique<internal::audio_player_impl_streaming>(data);
}

void audio_player::reset() {
    g_audio_player_manager::get().get_player(handle()).reset();
}

audio_player &audio_player::play_once() {
    g_audio_player_manager::get().get_player(handle())->play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    g_audio_player_manager::get().get_player(handle())->play_loop(loop_point);
    return *this;
}

void audio_player::pause() {
    g_audio_player_manager::get().get_player(handle())->pause();
}

void audio_player::resume() {
    g_audio_player_manager::get().get_player(handle())->resume();
}

void audio_player::stop() {
    g_audio_player_manager::get().get_player(handle())->stop();
}

void audio_player::seek(std::chrono::nanoseconds point) {
    g_audio_player_manager::get().get_player(handle())->seek(point);
}

std::chrono::nanoseconds audio_player::pos() const {
    return g_audio_player_manager::get().get_player(handle())->pos();
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
