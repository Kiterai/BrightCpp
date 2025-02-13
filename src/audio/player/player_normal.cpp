#include "player_normal.hpp"
#include "../../global_module.hpp"
#include "../audio_asset_manager.hpp"
#include <iostream>

BRIGHTCPP_START

namespace internal {

using g_audio_mixer = internal::global_module<internal::audio_mixer>;
using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;

audio_player_impl_normal::audio_player_impl_normal(audio &new_data) {
    data = new_data;

    auto info = g_audio_asset_manager::get().get_info(data->handle());
    buf_begin = info.begin;
    buf_end = info.end;

    context_id = g_audio_mixer::get().add_playing(
        internal::audio_play_info{
            .delay_timer = 0,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::normal,
            .stopped = false,
            .paused = true,
        });
}

audio_player_impl_normal::~audio_player_impl_normal() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = true,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_normal::play_once() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buf_begin,
            .end_pos = buf_end,
            .loop_pos = buf_begin,
            .next_loop_end_pos = buf_end,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::normal,
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

} // namespace internal

BRIGHTCPP_END
