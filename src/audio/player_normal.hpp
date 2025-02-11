#pragma once

#include "mixer.hpp"
#include <brightcpp/audio.hpp>
#include <chrono>
#include <optional>
#include <unordered_map>

BRIGHTCPP_START

namespace internal {

class audio_player_impl_normal {
    std::optional<audio> data;
    internal::audio_context_id context_id;

    const float *buf_begin, *buf_end;
    bool streaming = false;

  public:
    audio_player_impl_normal();
    void set(audio &new_data);
    void play_once();
    void play_loop(std::chrono::nanoseconds loop_point);
    void reset();
    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    std::chrono::nanoseconds pos() const;
};

extern std::unordered_map<handle_holder<audio_player>::handle_value_t, audio_player_impl_normal> players;
handle_holder<audio_player>::handle_value_t player_register();

} // namespace internal

BRIGHTCPP_END
