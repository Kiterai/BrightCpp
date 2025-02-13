#include "player_streaming.hpp"
#include "../../global_module.hpp"
#include "../streaming_audio_asset_manager.hpp"
#include "../streaming_manager.hpp"
#include <iostream>

BRIGHTCPP_START

namespace internal {

using g_audio_mixer = internal::global_module<internal::audio_mixer>;
using g_streaming_manager = internal::global_module<internal::streaming_manager>;
using g_streaming_audio_manager = internal::global_module<internal::streaming_audio_asset_manager>;

constexpr size_t buffer_cap = 48000;

audio_player_impl_streaming::audio_player_impl_streaming(streaming_audio &data)
    : data{data},
      buffer(buffer_cap * 2),
      raw_buffer(buffer_cap),
      buffer_state{0},
      resampler{44100, 48000} {
    context_id = g_audio_mixer::get().add_playing(
        internal::audio_play_info{
            .delay_timer = 0,
            .volume = 1.0f,
            .mode = internal::audio_play_info::play_mode::streaming_loop_available,
            .stopped = false,
            .paused = true,
        });
    auto &loader = g_streaming_audio_manager::get().get_loader(data.handle());

    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[0] = resampler.resample(buffer.data(), raw_buffer.data(), loaded, buffer_cap);
    }
    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[1] = resampler.resample(buffer.data() + buffer_cap, raw_buffer.data(), loaded, buffer_cap);
    }

    g_streaming_manager::get().register_loader(context_id, [this] {
        auto &loader = g_streaming_audio_manager::get().get_loader(this->data->handle());

        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        if (loop && loaded < req) {
            loader.seek(loop_frame);
            loaded += loader.load_chunk(raw_buffer.data() + loaded, req - loaded);
        }

        bufsz[buffer_state] = resampler.resample(buffer.data() + (buffer_state * buffer_cap), raw_buffer.data(), loaded, buffer_cap);

        if (loaded >= req) {
            g_audio_mixer::get().set_playing(
                context_id,
                internal::audio_play_info{
                    .loop_pos = buffer.data() + (buffer_state * buffer_cap),
                    .next_loop_end_pos = buffer.data() + (buffer_state * buffer_cap) + bufsz[buffer_state],
                    .mode = internal::audio_play_info::play_mode::streaming_loop_available,
                },
                internal::audio_play_update_bit::next_range |
                    internal::audio_play_update_bit::mode);
            buffer_state = (buffer_state + 1) % 2;
        } else {
            g_audio_mixer::get().set_playing(
                context_id,
                internal::audio_play_info{
                    .loop_pos = buffer.data() + (buffer_state * buffer_cap),
                    .next_loop_end_pos = buffer.data() + (buffer_state * buffer_cap) + bufsz[buffer_state],
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
    loop = false;
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buffer.data(),
            .end_pos = buffer.data() + bufsz[0],
            .loop_pos = buffer.data() + buffer_cap,
            .next_loop_end_pos = buffer.data() + buffer_cap + bufsz[1],
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

void audio_player_impl_streaming::play_loop(std::chrono::nanoseconds loop_point) {
    loop = true;
    loop_frame = loop_point.count() * 48000 / 1'000'000'000;
    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .delay_timer = 0,
            .current_pos = buffer.data(),
            .end_pos = buffer.data() + bufsz[0],
            .loop_pos = buffer.data() + buffer_cap,
            .next_loop_end_pos = buffer.data() + buffer_cap + bufsz[1],
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

void audio_player_impl_streaming::stop() {
    auto &loader = g_streaming_audio_manager::get().get_loader(this->data->handle());
    loader.seek(0);

    buffer_state = 0;
    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[0] = resampler.resample(buffer.data(), raw_buffer.data(), loaded, buffer_cap);
    }
    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[1] = resampler.resample(buffer.data() + buffer_cap, raw_buffer.data(), loaded, buffer_cap);
    }

    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
}
void audio_player_impl_streaming::seek(std::chrono::nanoseconds point) {
    auto &loader = g_streaming_audio_manager::get().get_loader(this->data->handle());
    loader.seek(point.count() * 48000 / 1'000'000'000);

    buffer_state = 0;
    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[0] = resampler.resample(buffer.data(), raw_buffer.data(), loaded, buffer_cap);
    }
    {
        size_t req = buffer_cap * 44100 / 48000 - 128;
        size_t loaded = loader.load_chunk(raw_buffer.data(), req);
        bufsz[1] = resampler.resample(buffer.data() + buffer_cap, raw_buffer.data(), loaded, buffer_cap);
    }

    g_audio_mixer::get().set_playing(
        context_id,
        internal::audio_play_info{
            .stopped = false,
            .paused = true,
        },
        internal::audio_play_update_bit::stop_pause);
}
// std::chrono::nanoseconds audio_player_impl_streaming::pos() const {
//     // TODO
// }

} // namespace internal

BRIGHTCPP_END
