#pragma once

#include "streaming_manager.hpp"
#include "../global_module.hpp"
#include "mixer.hpp"
#include <chrono>

BRIGHTCPP_START

namespace internal {

streaming_manager::streaming_manager() {
    streaming_thread = std::thread([this]() {
        const auto &default_mixer = global_module<audio_mixer>::get();

        while (running) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                for (const auto &ctx : streams)
                    if (default_mixer.get_playing(ctx.id).mode ==
                        audio_play_info::play_mode::streaming_loop_invalid)
                        ctx.loader();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

streaming_manager::~streaming_manager() {
    running = false;
    streaming_thread.join();
}

void streaming_manager::register_loader(audio_context_id id, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(mtx);
    streams.push_back(streaming_context{
        .id = id,
        .loader = std::move(callback),
    });
}

void streaming_manager::unregister_loader(audio_context_id id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::erase_if(streams, [id](const streaming_context &ctx) {
        return id == ctx.id;
    });
}

} // namespace internal

BRIGHTCPP_END
