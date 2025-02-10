#pragma once

#include <brightcpp/common.hpp>
#include <vector>

BRIGHTCPP_START

namespace internal {

using audio_context_id = size_t;

struct audio_buffer_play_info {
    int delay_timer;
    const float *current_pos, *end_pos, *loop_pos, *next_loop_end_pos;
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

    using streaming_callback = void(void *, audio_buffer_play_info *);
    void *callback_data;
    streaming_callback callback;
};

class audio_mixer {
    std::vector<audio_buffer_play_info> playing_list;
    audio_context_id id_serial_count = 0;

  public:
    audio_context_id add_playing(const audio_buffer_play_info &);
    void set_playing(audio_context_id id, const audio_buffer_play_info &);
    audio_buffer_play_info get_playing(audio_context_id id) const;
    void read(float *dst, size_t frames);
};

} // namespace internal

BRIGHTCPP_END
