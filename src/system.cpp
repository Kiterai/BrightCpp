#include <iostream>
#include <optional>

#include "audio/audio_asset_manager.hpp"
#include "audio/audio_player_manager.hpp"
#include "audio/mixer.hpp"
#include "audio/streaming_audio_asset_manager.hpp"
#include "audio/streaming_manager.hpp"
#include "glfw/glfw.hpp"
#include "global_module.hpp"
#include "libsoundio/audio.hpp"
#include "vulkan/graphics.hpp"
#include "vulkan/renderer2d.hpp"
#include "vulkan/texture.hpp"

BRIGHTCPP_START

namespace internal {

static size_t initializer_count = 0;

struct global_objects_t {
    std::unique_ptr<os_util_backend> os_util;

    std::unique_ptr<vulkan::graphics_vulkan> graphics;
    std::unique_ptr<vulkan::texture_factory_vulkan> tex_factory;
    std::unique_ptr<vulkan::renderer2d_factory_vulkan> renderer2d_factory;

    std::optional<audio_asset_manager> audio_loader_obj;
    std::optional<streaming_audio_asset_manager> streaming_audio_manager_obj;
    std::optional<audio_mixer> audio_mixer;
    std::optional<streaming_manager> streaming_manager;
    std::unique_ptr<audio_backend> audio;
    std::optional<audio_player_manager> player_manager;
};

global_objects_t global_objects;

template <>
os_util_backend *global_module_constructor<os_util_backend>() {
    global_objects.os_util = glfw::make_glfw_manager();
    return global_objects.os_util.get();
}

template <>
vulkan::graphics_vulkan *global_module_constructor<vulkan::graphics_vulkan>() {
    global_objects.graphics = std::make_unique<vulkan::graphics_vulkan>();
    return global_objects.graphics.get();
}
template <>
graphics_backend *global_module_constructor<graphics_backend>() {
    return &global_module<vulkan::graphics_vulkan>::get();
}

template <>
vulkan::texture_factory_vulkan *global_module_constructor<vulkan::texture_factory_vulkan>() {
    global_objects.tex_factory = std::make_unique<vulkan::texture_factory_vulkan>();
    return global_objects.tex_factory.get();
}
template <>
texture_factory_backend *global_module_constructor<texture_factory_backend>() {
    return &global_module<vulkan::texture_factory_vulkan>::get();
}

template <>
vulkan::renderer2d_factory_vulkan *global_module_constructor<vulkan::renderer2d_factory_vulkan>() {
    global_objects.renderer2d_factory = std::make_unique<vulkan::renderer2d_factory_vulkan>();
    return global_objects.renderer2d_factory.get();
}
template <>
renderer2d_factory_backend *global_module_constructor<renderer2d_factory_backend>() {
    return &global_module<vulkan::renderer2d_factory_vulkan>::get();
}

template <>
audio_asset_manager *global_module_constructor<audio_asset_manager>() {
    global_objects.audio_loader_obj = audio_asset_manager{};
    return &*global_objects.audio_loader_obj;
}

template <>
streaming_audio_asset_manager *global_module_constructor<streaming_audio_asset_manager>() {
    global_objects.streaming_audio_manager_obj.emplace();
    return &*global_objects.streaming_audio_manager_obj;
}

template <>
audio_mixer *global_module_constructor<audio_mixer>() {
    global_objects.audio_mixer.emplace();
    return &*global_objects.audio_mixer;
}

template <>
audio_player_manager *global_module_constructor<audio_player_manager>() {
    global_objects.player_manager.emplace();
    return &*global_objects.player_manager;
}

template <>
streaming_manager *global_module_constructor<streaming_manager>() {
    global_objects.streaming_manager.emplace();
    return &*global_objects.streaming_manager;
}

template <>
audio_backend *global_module_constructor<audio_backend>() {
    global_objects.audio = libsoundio::make_libsoundio_manager();
    return global_objects.audio.get();
}

} // namespace internal

BRIGHTCPP_END
