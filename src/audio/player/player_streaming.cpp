#include "player_streaming.hpp"
#include "../../global_module.hpp"
#include "../streaming_audio_manager.hpp"
#include "../streaming_manager.hpp"
#include <iostream>

BRIGHTCPP_START

namespace internal {

using g_audio_mixer = internal::global_module<internal::audio_mixer>;
using g_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_streaming_audio_manager = internal::global_module<internal::streaming_audio_manager>;

constexpr size_t buffer_sz = 48000;

audio_player_impl_streaming::audio_player_impl_streaming(streaming_audio &data)
    : data{data},
      buffer(buffer_sz * 2),
      buffer_state{0} {
    context_id = g_audio_mixer::get().add_playing(
        internal::audio_play_info{
            .delay_timer = 0,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::streaming_loop_available,
            .stopped = false,
            .paused = true,
        });
    auto &loader = g_streaming_audio_manager::get().get_loader(data.handle());
    loader.load_chunk(buffer.data(), buffer_sz * 2);

    g_streaming_manager::get().register_loader(context_id, [this] {
        auto &loader = g_streaming_audio_manager::get().get_loader(this->data->handle());
        auto loaded = loader.load_chunk(buffer.data() + (buffer_state * buffer_sz), buffer_sz);

        if (loaded >= buffer_sz) {
            g_audio_mixer::get().set_playing(
                context_id,
                internal::audio_play_info{
                    .loop_pos = buffer.data() + (buffer_state * buffer_sz),
                    .next_loop_end_pos = buffer.data() + (buffer_state * buffer_sz) + buffer_sz,
                    .mode = internal::audio_play_info::play_mode::streaming_loop_available,
                },
                internal::audio_play_update_bit::next_range |
                    internal::audio_play_update_bit::mode);
            buffer_state = (buffer_state + 1) % 2;
        } else {
            g_audio_mixer::get().set_playing(
                context_id,
                internal::audio_play_info{
                    .loop_pos = buffer.data() + (buffer_state * buffer_sz),
                    .next_loop_end_pos = buffer.data() + (buffer_state * buffer_sz) + loaded,
                    .mode = internal::audio_play_info::play_mode::streaming_loop_nextend,
                },
                internal::audio_play_update_bit::next_range |
                    internal::audio_play_update_bit::mode);
        }
    });
}

audio_player_impl_streaming::~audio_player_impl_streaming() {
    g_streaming_manager::get().unregister_loader(context_id);
}

void audio_player_impl_streaming::play_once() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buffer.data(),
            .end_pos = buffer.data() + buffer_sz,
            .loop_pos = buffer.data() + buffer_sz,
            .next_loop_end_pos = buffer.data() + buffer_sz + buffer_sz,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::streaming_loop_available,
            .stopped = false,
            .paused = false,
        },
        internal::audio_play_update_bit::delay_timer |
            internal::audio_play_update_bit::current_range |
            internal::audio_play_update_bit::next_range |
            internal::audio_play_update_bit::mode |
            internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_streaming::pause() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
}

void audio_player_impl_streaming::resume() {
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = false,
        },
        internal::audio_play_update_bit::stop_pause);
}

} // namespace internal

BRIGHTCPP_END
