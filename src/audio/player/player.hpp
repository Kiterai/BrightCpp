#pragma once

#include <brightcpp/common.hpp>
#include <filesystem>
#include <stdexcept>
#include <vector>

BRIGHTCPP_START

namespace internal {

class audio_player_backend {
  public:
    virtual ~audio_player_backend() = default;

    virtual void play_once() { throw std::runtime_error("play_once() unsupported"); };
    virtual void play_loop(std::chrono::nanoseconds loop_point) { throw std::runtime_error("play_loop() unsupported"); };
    virtual void pause() { throw std::runtime_error("pause() unsupported"); };
    virtual void resume() { throw std::runtime_error("resume() unsupported"); };
    virtual void stop() { throw std::runtime_error("stop() unsupported"); };
    virtual void seek(std::chrono::nanoseconds point) { throw std::runtime_error("seek() unsupported"); };
    virtual std::chrono::nanoseconds pos() const { throw std::runtime_error("pos() unsupported"); };
};

} // namespace internal

BRIGHTCPP_END
