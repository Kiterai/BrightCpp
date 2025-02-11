#pragma once

#include <brightcpp/common.hpp>
#include <filesystem>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_player_backend {
  public:
    virtual ~audio_player_backend() = default;

    virtual void play_once() { throw std::exception("play_once() unsupported"); };
    virtual void play_loop(std::chrono::nanoseconds loop_point) { throw std::exception("play_loop() unsupported"); };
    virtual void pause() { throw std::exception("pause() unsupported"); };
    virtual void resume() { throw std::exception("resume() unsupported"); };
    virtual void stop() { throw std::exception("stop() unsupported"); };
    virtual void seek(std::chrono::nanoseconds point) { throw std::exception("seek() unsupported"); };
    virtual std::chrono::nanoseconds pos() const { throw std::exception("pos() unsupported"); };
};

} // namespace internal

BRIGHTCPP_END
