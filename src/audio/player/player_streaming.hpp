#pragma once

#include "../mixer.hpp"
#include "../resample.hpp"
#include "player.hpp"
#include <brightcpp/audio.hpp>
#include <chrono>
#include <optional>

BRIGHTCPP_START

namespace internal {

class audio_player_impl_streaming : public audio_player_backend {
    std::optional<streaming_audio> data;
    internal::audio_context_id context_id;
    std::vector<float> buffer, raw_buffer;
    int buffer_state = 0;
    streaming_resampler resampler;
    size_t bufsz[2];
    size_t loop_frame;
    bool loop = false;

  public:
    audio_player_impl_streaming(streaming_audio &new_data);
    ~audio_player_impl_streaming();
    void play_once();
    void play_loop(std::chrono::nanoseconds loop_point);
    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    // std::chrono::nanoseconds pos() const;
};

} // namespace internal

BRIGHTCPP_END
