#include <iostream>
#include <optional>

#include "audio/loader/audio_loader.hpp"
#include "glfw/glfw.hpp"
#include "global_module.hpp"
#include "libsoundio/audio.hpp"
#include "vulkan/registration.hpp"

BRIGHTCPP_START

namespace internal {

static size_t initializer_count = 0;

struct global_objects_t {
    std::shared_ptr<os_util_backend> os_util;
    std::unique_ptr<object_container> graphics;
    std::unique_ptr<audio_backend> audio;
    std::optional<audio_loader> audio_loader;
};

std::optional<global_objects_t> global_objects;

} // namespace internal

using namespace internal;

system_initializer::system_initializer() {
    if (initializer_count == 0) {
        global_objects.emplace();

        global_objects->os_util = glfw::make_glfw_manager();
        global_module<os_util_backend>::set(*global_objects->os_util);

        global_objects->graphics = vulkan::register_objects();

        global_objects->audio = libsoundio::make_libsoundio_manager();
        global_module<audio_backend>::set(*global_objects->audio);

        global_objects->audio_loader = audio_loader{};
        global_module<audio_loader>::set(*global_objects->audio_loader);

#ifdef _DEBUG
        std::cout << "successfully initialized BrightCpp." << std::endl;
#endif
    }
    initializer_count++;
}

system_initializer::~system_initializer() {
    initializer_count--;
    if (initializer_count == 0) {
#ifdef _DEBUG
        std::cout << "shutdown BrightCpp..." << std::endl;
#endif
        global_objects.reset();
    }
}

BRIGHTCPP_END
