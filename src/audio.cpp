#include "interfaces/audio.hpp"
#include "global_module.hpp"
#include <brightcpp/audio.hpp>
#include <iostream>
#include <unordered_map>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

BRIGHTCPP_START

using g_audio = internal::global_module<internal::audio_backend>;

std::vector<std::vector<float>> loaded_audios;

handle_holder<audio>::handle_value_t easy_audio_loader(const char *path, audio_file_type type) {
    // TODO
    FILE *fp = std::fopen(path, "rb");

    OggVorbis_File vf;
    if (ov_open(fp, &vf, NULL, 0) < 0)
        throw std::runtime_error("failed to ov_open");

    auto vi = ov_info(&vf, -1);
    if (vi == nullptr)
        throw std::runtime_error("failed to ov_info");

    std::vector<float> loaded_audio;

    std::vector<char> buffer(4096);
    auto p = reinterpret_cast<int16_t *>(buffer.data());
    int cur, decoded;
    do {
        decoded = ov_read(&vf, buffer.data(), 4096, 0, 2, 1, &cur);
        for (int i = 0; i < decoded / (2 * sizeof(int16_t)); i++)
            loaded_audio.push_back(float(p[i * 2]) / 32768);
    } while (decoded > 0);

    ov_clear(&vf);
    fclose(fp);

    int id = loaded_audios.size();
    loaded_audios.push_back(std::move(loaded_audio));
    return id;
}

audio::audio(const char *path, audio_file_type type) : handle_holder{easy_audio_loader(path, type)} {}

class audio_player_impl {
    std::optional<audio> data;
    internal::audio_context_id context_id;

  public:
    audio_player_impl() {
        context_id = g_audio::get().play_audio_buffer(
            internal::audio_buffer_play_info{
                .delay_timer = 0,
                // .current_pos = buffers[audio_handle].data(),
                // .end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                // .loop_pos = buffers[audio_handle].data(),
                // .next_loop_end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_buffer_play_info::play_mode::normal,
                .stopped = false,
                .paused = true,
            });
    }
    void set(audio &new_data) {
        data = new_data;
    }
    void play_once() {
        auto audio_handle = data->handle();

        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .delay_timer = 0,
                .current_pos = loaded_audios[audio_handle].data(),
                .end_pos = loaded_audios[audio_handle].data() + loaded_audios[audio_handle].size(),
                .loop_pos = loaded_audios[audio_handle].data(),
                .next_loop_end_pos = loaded_audios[audio_handle].data() + loaded_audios[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_buffer_play_info::play_mode::normal,
                .stopped = false,
                .paused = false,
            });
    }
    void play_loop(std::chrono::nanoseconds loop_point) {
        auto audio_handle = data->handle();

        auto loop_point_sampleindex = loop_point.count() * 48000 / 1'000'000'000;

        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .delay_timer = 0,
                .current_pos = loaded_audios[audio_handle].data(),
                .end_pos = loaded_audios[audio_handle].data() + loaded_audios[audio_handle].size(),
                .loop_pos = loaded_audios[audio_handle].data() + loop_point_sampleindex,
                .next_loop_end_pos = loaded_audios[audio_handle].data() + loaded_audios[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_buffer_play_info::play_mode::loop,
                .stopped = false,
                .paused = false,
            });
    }
    void reset() {
        // TODO
        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .stopped = true,
            });
    }
    void pause() {
        // TODO
        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .paused = true,
            });
    }
    void resume() {
        // TODO
        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .paused = false,
            });
    }
    void stop() {
        // TODO
        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .paused = true,
            });
    }
    void seek(std::chrono::nanoseconds point) {
        // TODO
        auto audio_handle = data->handle();
        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .current_pos = loaded_audios[audio_handle].data(),
                .paused = true,
            });
    }
    std::chrono::nanoseconds pos() const {
        // TODO
        return std::chrono::nanoseconds(0);
    }
};

handle_holder<audio>::handle_value_t player_serial_count = 0;
std::unordered_map<handle_holder<audio>::handle_value_t, audio_player_impl> players;

handle_holder<audio>::handle_value_t player_register() {
    int new_id = player_serial_count;
    player_serial_count++;
    players.emplace(new_id, audio_player_impl{});
    return new_id;
}

audio_player::audio_player(audio &data) : handle_holder{player_register()} {
    players[handle()].set(data);
}

void audio_player::set(audio &data) {
    players[handle()].set(data);
}

void audio_player::reset() {
    players[handle()].reset();
}

audio_player &audio_player::play_once() {
    players[handle()].play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    players[handle()].play_loop(loop_point);
    return *this;
}

void audio_player::pause() {
    players[handle()].pause();
}

void audio_player::resume() {
    players[handle()].resume();
}

void audio_player::stop() {
    players[handle()].stop();
}

void audio_player::seek(std::chrono::nanoseconds point) {
    players[handle()].seek(point);
}

std::chrono::nanoseconds audio_player::pos() const {
    return players[handle()].pos();
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
