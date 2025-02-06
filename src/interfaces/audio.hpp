#pragma once

#include <brightcpp/common.hpp>
#include <span>

BRIGHTCPP_START

namespace internal {

using audio_context_id = size_t;

struct audio_buffer_play_info {
    int delay_timer;
    float *current_pos, end_pos, loop_pos, next_loop_end_pos;
    float volume;
    audio_context_id id;
    enum class play_mode {
        normal,
        oneshot,
        loop,
        streaming_loop_invalid,
        streaming_loop_available,
    } mode;
    bool stopped, paused;
};

class audio_backend {
  public:
    virtual ~audio_backend() = default;

    virtual int get_sample_rate() = 0;

    virtual audio_context_id play_audio_buffer(const audio_buffer_play_info &) = 0;
    virtual void set_playing_state(audio_context_id id, const audio_buffer_play_info &) = 0;
};

} // namespace internal

BRIGHTCPP_END
