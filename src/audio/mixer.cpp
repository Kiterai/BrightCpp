#include "mixer.hpp"
#include <iostream>
#include <stdexcept>

BRIGHTCPP_START

namespace internal {

audio_context_id audio_mixer::add_playing(const audio_play_info &info) {
    playing_list.push_back(info);
    auto new_id = playing_list.back().id = id_serial_count;
    id_serial_count++;
    return new_id;
}

void audio_mixer::set_playing(audio_context_id id, const audio_play_info &info, audio_play_update_mask mask) {
    for (auto &playing : playing_list) {
        if (playing.id == id) {
            if (mask & audio_play_update_bit::delay_timer) {
                playing.delay_timer = info.delay_timer;
            }
            if (mask & audio_play_update_bit::current_range) {
                playing.current_pos = info.current_pos;
                playing.end_pos = info.end_pos;
            }
            if (mask & audio_play_update_bit::next_range) {
                playing.loop_pos = info.loop_pos;
                playing.next_loop_end_pos = info.next_loop_end_pos;
            }
            if (mask & audio_play_update_bit::mode) {
                playing.mode = info.mode;
            }
            if (mask & audio_play_update_bit::stop_pause) {
                playing.stopped = info.stopped;
                playing.paused = info.paused;
            }
            playing.id = id;
            break;
        }
    }
}

audio_play_info audio_mixer::get_playing(audio_context_id id) const {
    for (auto &playing : playing_list) {
        if (playing.id == id)
            return playing;
    }
    throw std::runtime_error("invalid audio_context id");
}

void audio_mixer::read(float *dst, size_t frames) {
    for (int frame = 0; frame < frames; frame++) {
        float sample = 0;
        for (auto &playing : playing_list) {
            if (playing.paused)
                continue;
            sample += *playing.current_pos;
            playing.current_pos++;
            if (playing.current_pos >= playing.end_pos) {
                switch (playing.mode) {
                case audio_play_info::play_mode::normal:
                    playing.current_pos = playing.loop_pos;
                    playing.paused = true;
                    break;
                case audio_play_info::play_mode::oneshot:
                    playing.stopped = playing.paused = true;
                    break;
                case audio_play_info::play_mode::loop:
                    playing.current_pos = playing.loop_pos;
                    playing.end_pos = playing.next_loop_end_pos;
                    break;
                case audio_play_info::play_mode::streaming_loop_invalid:
                    playing.paused = true;
                    break;
                case audio_play_info::play_mode::streaming_loop_available:
                    playing.current_pos = playing.loop_pos;
                    playing.end_pos = playing.next_loop_end_pos;
                    playing.mode = audio_play_info::play_mode::streaming_loop_invalid;
                    break;
                case audio_play_info::play_mode::streaming_loop_nextend:
                    playing.current_pos = playing.loop_pos;
                    playing.end_pos = playing.next_loop_end_pos;
                    playing.mode = audio_play_info::play_mode::normal;
                    break;
                }
            }
        }

        *dst = sample;
        dst++;
    }

    std::erase_if(playing_list, [](const auto &playing) {
        return playing.stopped;
    });
}

} // namespace internal

BRIGHTCPP_END
