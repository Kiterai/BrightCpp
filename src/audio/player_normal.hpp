#pragma once

#include "mixer.hpp"
#include "player.hpp"
#include <brightcpp/audio.hpp>
#include <chrono>
#include <optional>

BRIGHTCPP_START

namespace internal {

class audio_player_impl_normal : public audio_player_backend {
    std::optional<audio> data;
    internal::audio_context_id context_id;

    const float *buf_begin, *buf_end;
    bool streaming = false;

  public:
    audio_player_impl_normal(audio &new_data);
    void play_once();
    void play_loop(std::chrono::nanoseconds loop_point);
    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    std::chrono::nanoseconds pos() const;
};

} // namespace internal

BRIGHTCPP_END
