#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>
#include <chrono>
#include <vector>

namespace BRIGHTCPP_NAMESPACE {

struct audio_sample {
}; 

class audio_buffer {
  public:
    void write(audio_sample *samples, size_t length);
    void read(audio_sample *samples, size_t length);
    void cread(audio_sample *samples, size_t length, size_t pos = 0) const;
    size_t len_samples() const;
    size_t len_bytes() const;
};

class audio_out {
};

class audio_filter {
  public:
};

class audio_impl;

class audio_clip : handle_holder<audio_impl> {
    size_t sample_begin, sample_end;

  public:
};

class audio : handle_holder<audio_impl> {
  public:
    audio_clip clip(std::chrono::nanoseconds start);
    audio_clip clip(std::chrono::nanoseconds start, std::chrono::nanoseconds end);
};

class audio_streaming : handle_holder<audio_streaming> {
    audio_buffer stream_buf;

  public:
    audio_buffer &buffer() { return stream_buf; }

    void play();
    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    std::chrono::nanoseconds pos() const;

    bool loading() const;
};

class audio_player {
  public:
    audio_player(audio_buffer &output);

    void set(audio &);
    void set(audio_clip &);
    void set(audio_streaming &);
    void set(audio_buffer &buf);
    void reset();

    audio_player &play_once();
    audio_player &play_loop(std::chrono::nanoseconds loop_point = std::chrono::nanoseconds(0)) {
    }

    audio_filter &filter();
    audio_filter &filter(audio_filter &filter);

    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    std::chrono::nanoseconds pos() const;
    bool busy() const;
};

class audio_player_multi {
    std::vector<audio_player> players;

  public:
    audio_player_multi(audio_buffer &output, size_t num);

    audio_player &play_once(audio &);
    audio_player &play_once(audio_clip &);
    audio_player &bind(audio_buffer &buf);

    audio_filter &filter();
    audio_filter &filter(audio_filter &filter);

    void pause();
    void resume();
    void stop();
    std::vector<std::chrono::nanoseconds> pos() const;
    std::vector<bool> busy() const;
    bool busy_all() const;
    bool busy_any() const;
};

} // namespace BRIGHTCPP_NAMESPACE
