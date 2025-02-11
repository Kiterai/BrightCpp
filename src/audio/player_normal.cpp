#include "player_normal.hpp"
#include "../global_module.hpp"
#include "audio_asset_manager.hpp"
#include "streaming_manager.hpp"
#include <iostream>

BRIGHTCPP_START

namespace internal {

using g_audio_mixer = internal::global_module<internal::audio_mixer>;
using g_audio_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;

audio_player_impl_normal::audio_player_impl_normal() {
    context_id = g_audio_mixer::get().add_playing(
        internal::audio_play_info{
            .delay_timer = 0,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::normal,
            .stopped = false,
            .paused = true,
        });
}

void audio_player_impl_normal::set(audio &new_data) {
    if (streaming)
        g_audio_streaming_manager::get().unregister_loader(context_id);

    data = new_data;

    auto info = g_audio_asset_manager::get().get_info(data->handle());
    buf_begin = info.begin;
    buf_end = info.end;

    if (streaming)
        g_audio_streaming_manager::get().register_loader(context_id, []() {
            // TODO;
        });
}

void audio_player_impl_normal::play_once() {
    auto mode = streaming ? internal::audio_play_info::play_mode::streaming_loop_available
                          : internal::audio_play_info::play_mode::normal;

    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buf_begin,
            .end_pos = buf_end,
            .loop_pos = buf_begin,
            .next_loop_end_pos = buf_end,
            .volume = 1.0f,
            .mode = mode,
            .stopped = false,
            .paused = false,
        },
        internal::audio_play_update_bit::delay_timer |
            internal::audio_play_update_bit::current_range |
            internal::audio_play_update_bit::next_range |
            internal::audio_play_update_bit::mode |
            internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::play_loop(std::chrono::nanoseconds loop_point) {
    auto loop_point_sampleindex = loop_point.count() * 48000 / 1'000'000'000;

    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buf_begin,
            .end_pos = buf_end,
            .loop_pos = buf_begin + loop_point_sampleindex,
            .next_loop_end_pos = buf_end,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::loop,
            .stopped = false,
            .paused = false,
        },
        internal::audio_play_update_bit::delay_timer |
            internal::audio_play_update_bit::current_range |
            internal::audio_play_update_bit::next_range |
            internal::audio_play_update_bit::mode |
            internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::reset() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = true,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
    g_audio_streaming_manager::get().unregister_loader(context_id);
}

void audio_player_impl_normal::pause() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::resume() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = false,
        },
        internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::stop() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .current_pos = buf_begin,
            .end_pos = buf_end,
            .stopped = false,
            .paused = true,
        },
        internal::audio_play_update_bit::current_range |
            internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::seek(std::chrono::nanoseconds point) {
    auto seek_point_sampleindex = point.count() * 48000 / 1'000'000'000;

    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .current_pos = buf_begin + seek_point_sampleindex,
            .end_pos = buf_end,
        },
        internal::audio_play_update_bit::current_range);
}

std::chrono::nanoseconds audio_player_impl_normal::pos() const {
    auto s = g_audio_mixer::get().get_playing(context_id).current_pos - buf_begin;
    return std::chrono::nanoseconds(s * 1'000'000'000 / 48000);
}

handle_holder<audio_player>::handle_value_t player_serial_count = 0;
std::unordered_map<handle_holder<audio_player>::handle_value_t, audio_player_impl_normal> players;

handle_holder<audio_player>::handle_value_t player_register() {
    auto new_id = player_serial_count;
    player_serial_count++;
    players.emplace(new_id, audio_player_impl_normal{});
    return new_id;
}

} // namespace internal

BRIGHTCPP_END
