#pragma once

#include "mixer.hpp"
#include <brightcpp/audio.hpp>
#include <brightcpp/common.hpp>
#include <functional>
#include <thread>

BRIGHTCPP_START

namespace internal {

class streaming_manager {
    struct streaming_context {
        audio_context_id id;
        std::function<void()> loader;
    };
    std::vector<streaming_context> streams;
    std::thread streaming_thread;
    bool running = true;

  public:
    streaming_manager();
    ~streaming_manager();
    void register_loader(audio_context_id id, std::function<void()> callback);
    void unregister_loader(audio_context_id id);
};

} // namespace internal

BRIGHTCPP_END
