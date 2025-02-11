#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/handle_holder.hpp>
#include <chrono>
#include <vector>

BRIGHTCPP_START

enum class audio_file_type {
    auto_detect,
    wav,
    ogg,
    mp3,
    flac,
};

class audio : public handle_holder<audio> {
  public:
    audio(const char *path, audio_file_type type = audio_file_type::auto_detect);
};

class streaming_audio : public handle_holder<streaming_audio> {
  public:
    streaming_audio(const char *path, audio_file_type type = audio_file_type::auto_detect);
};

class audio_player : public handle_holder<audio_player> {
  public:
    audio_player(audio &data);
    audio_player(streaming_audio &data);

    void set(audio &);
    void reset();

    audio_player &play_once();
    audio_player &play_loop(std::chrono::nanoseconds loop_point = std::chrono::nanoseconds(0));

    void pause();
    void resume();
    void stop();
    void seek(std::chrono::nanoseconds point);
    std::chrono::nanoseconds pos() const;
    bool busy() const;
};

class oneshot_audio_player {
  public:
    void play_oneshot(audio &);

    void pause();
    void resume();
    void stop();
    bool busy_any() const;
};

BRIGHTCPP_END
