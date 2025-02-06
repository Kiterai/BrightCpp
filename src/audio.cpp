#include "audio.hpp"
#include "global_module.hpp"
#include "interfaces/audio.hpp"
#include <brightcpp/audio.hpp>
#include <unordered_map>

BRIGHTCPP_START

using g_audio = internal::global_module<internal::audio_backend>;

std::vector<std::vector<float>> buffers;

handle_holder<audio> easy_audio_loader(char *path, audio_file_type type) {
    // TODO
}

audio::audio(char *path, audio_file_type type) : handle_holder{easy_audio_loader(path, type)} {}

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
                .current_pos = buffers[audio_handle].data(),
                .end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .loop_pos = buffers[audio_handle].data(),
                .next_loop_end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_buffer_play_info::play_mode::normal,
                .stopped = false,
                .paused = false,
            });
    }
    void play_loop() {
        auto audio_handle = data->handle();

        g_audio::get().set_playing_state(
            context_id,
            internal::audio_buffer_play_info{
                .delay_timer = 0,
                .current_pos = buffers[audio_handle].data(),
                .end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .loop_pos = buffers[audio_handle].data(),
                .next_loop_end_pos = buffers[audio_handle].data() + buffers[audio_handle].size(),
                .volume = 1.0f,
                .mode = internal::audio_buffer_play_info::play_mode::loop,
                .stopped = false,
                .paused = false,
            });
    }
};

handle_holder<audio>::handle_value_t player_serial_count = 0;
std::unordered_map<handle_holder<audio>::handle_value_t, audio_player_impl> players;

handle_holder<audio>::handle_value_t player_register() {
    players.emplace(player_serial_count, audio_player_impl{});
    player_serial_count++;
}

audio_player::audio_player(audio &data) : handle_holder{player_register()} {
    players[handle()].set(data);
}

void audio_player::set(audio &data) {
    players[handle()].set(data);
}

void audio_player::reset() {
    // TODO
}

audio_player &audio_player::play_once() {
    players[handle()].play_once();
    return *this;
}

audio_player &audio_player::play_loop(std::chrono::nanoseconds loop_point) {
    players[handle()].play_loop();
    return *this;
}

void audio_player::pause() {
    // TODO
}

void audio_player::resume() {
    // TODO
}

void audio_player::stop() {
    // TODO
}

void audio_player::seek(std::chrono::nanoseconds point) {
    // TODO
}

std::chrono::nanoseconds audio_player::pos() const {
    // TODO
    return std::chrono::nanoseconds(0);
}

bool audio_player::busy() const {
    // TODO
    return false;
}

BRIGHTCPP_END
