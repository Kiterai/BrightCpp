#include "interfaces/audio.hpp"
#include "audio/audio_asset_manager.hpp"
#include "audio/mixer.hpp"
#include "audio/streaming_manager.hpp"
#include "global_module.hpp"
#include <brightcpp/audio.hpp>
#include <iostream>

BRIGHTCPP_START

using g_audio_mixer = internal::global_module<internal::audio_mixer>;
using g_audio_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_audio_asset_manager = internal::global_module<internal::audio_asset_manager>;

audio::audio(const char *path, audio_file_type type) : handle_holder{g_audio_asset_manager::get().make(path, type)} {}

class audio_player_impl {
    std::optional<audio> data;
    internal::audio_context_id context_id;

    const float *buf_begin, *buf_end;
    bool streaming = false;

  public:
    audio_player_impl() {
        // ensure audio thread running
        internal::global_module<internal::audio_backend>::get();

        context_id = g_audio_mixer::get().add_playing(
            internal::audio_play_info{
                .delay_timer = 0,
                // .current_pos = buffers[audio_handle].data(),
                // .end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                // .loop_pos = buffers[audio_handle].data(),
                // .next_loop_end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_play_info::play_mode::normal,
                .stopped = false,
                .paused = true,
            });
    }
    void set(audio &new_data) {
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
    void play_once() {
        if (!streaming) {
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
        } else {
            g_audio_mixer::get().set_playing(
                context_id,
                internal::audio_play_info{
                    .delay_timer = 0,
                    .current_pos = buf_begin,
                    .end_pos = buf_end,
                    .loop_pos = buf_begin,
                    .next_loop_end_pos = buf_end,
                    .volume = 1.0f,
                    .mode = internal::audio_play_info::play_mode::streaming_loop_available,
                    .stopped = false,
                    .paused = false,
                },
                internal::audio_play_update_bit::delay_timer |
                    internal::audio_play_update_bit::current_range |
                    internal::audio_play_update_bit::next_range |
                    internal::audio_play_update_bit::mode |
                    internal::audio_play_update_bit::stop_pause);
        }
    }
    void play_loop(std::chrono::nanoseconds loop_point) {
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
    void reset() {
        g_audio_mixer::get().set_playing(
            context_id,
            internal::audio_play_info{
                .stopped = true,
                .paused = true,
            },
            internal::audio_play_update_bit::stop_pause);
        g_audio_streaming_manager::get().unregister_loader(context_id);
    }
    void pause() {
        g_audio_mixer::get().set_playing(
            context_id,
            internal::audio_play_info{
                .stopped = false,
                .paused = true,
            },
            internal::audio_play_update_bit::stop_pause);
    }
    void resume() {
        g_audio_mixer::get().set_playing(
            context_id,
            internal::audio_play_info{
                .stopped = false,
                .paused = false,
            },
            internal::audio_play_update_bit::stop_pause);
    }
    void stop() {
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
    void seek(std::chrono::nanoseconds point) {
        auto seek_point_sampleindex = point.count() * 48000 / 1'000'000'000;

        g_audio_mixer::get().set_playing(
            context_id,
            internal::audio_play_info{
                .current_pos = buf_begin + seek_point_sampleindex,
                .end_pos = buf_end,
            },
            internal::audio_play_update_bit::current_range);
    }
    std::chrono::nanoseconds pos() const {
        auto s = g_audio_mixer::get().get_playing(context_id).current_pos - buf_begin;
        return std::chrono::nanoseconds(s * 1'000'000'000 / 48000);
    }
};

handle_holder<audio>::handle_value_t player_serial_count = 0;
std::unordered_map<handle_holder<audio>::handle_value_t, audio_player_impl> players;

handle_holder<audio>::handle_value_t player_register() {
    auto new_id = player_serial_count;
    player_serial_count++;
    players.emplace(new_id, audio_player_impl{});
    return new_id;
}

audio_player::audio_player(audio &data) : handle_holder{player_register()} {
    players[handle()].set(data);
}

void audio_player::set(audio &data) {
    players[handle()].set(data);
}

void audio_player::reset() {
    players[handle()].reset();
}

audio_player &audio_player::play_once() {
    players[handle()].play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    players[handle()].play_loop(loop_point);
    return *this;
}

void audio_player::pause() {
    players[handle()].pause();
}

void audio_player::resume() {
    players[handle()].resume();
}

void audio_player::stop() {
    players[handle()].stop();
}

void audio_player::seek(std::chrono::nanoseconds point) {
    players[handle()].seek(point);
}

std::chrono::nanoseconds audio_player::pos() const {
    return players[handle()].pos();
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
