#pragma once

#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <span>

BRIGHTCPP_START

namespace internal {

struct audio_buffer_play_info {
    int delay_timer, current_pos, end_pos, loop_pos;
    float volume;
    bool loop;
};

using audio_context_id = int;

class audio_backend {
  public:
    virtual ~audio_backend() = default;

    virtual int get_sample_rate() = 0;

    virtual handle_holder<audio_impl>::handle_value_t create_audio_buffer(std::span<float> data) = 0;
    virtual void destroy_audio_buffer(handle_holder<audio_impl> &) = 0;
    virtual audio_context_id play_audio_buffer(handle_holder<audio_impl> &, const audio_buffer_play_info &) = 0;
    virtual void set_playing_state(audio_context_id id, const audio_buffer_play_info &) = 0;
};

} // namespace internal

BRIGHTCPP_END
