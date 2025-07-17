#pragma once

#include "rendertarget_factory.hpp"
#include "../../global_module.hpp"
#include "graphics.hpp"
#include "texture_rendertarget.hpp"
#include "window_rendertarget.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

rendertarget_factory_vulkan::~rendertarget_factory_vulkan() { global_module<graphics_vulkan>::get().wait_idle(); }

handle_holder<rendertarget>::handle_value_t rendertarget_factory_vulkan::create_render_target(window_backend &window) {
    const auto handle = rendertarget_db.size();

    const auto &g = global_module<graphics_vulkan>::get();

    rendertarget_db.insert({
        handle,
        std::make_unique<window_rendertarget_vulkan>(g.get_instance(), g.get_physdevice(), g.get_device(),
                                                     g.get_queue_indices(),
                                                     window.get_vulkan_surface(g.get_instance())),
    });
    return handle;
}

handle_holder<rendertarget>::handle_value_t
rendertarget_factory_vulkan::create_render_target(handle_holder<image_impl> image) {
    const auto handle = rendertarget_db.size();

    const auto &g = global_module<graphics_vulkan>::get();

    const auto &tex = global_module<texture_factory_vulkan>::get().get(image);

    rendertarget_db.insert({
        handle,
        std::make_unique<texture_rendertarget_vulkan>(
            g.get_device(),
            vk::Format::eR8G8B8A8Snorm, // TODO
            vk::Extent2D(tex.w_int, tex.h_int),
            std::vector<vk::Image>{ tex.image.get() },
            g.get_queue_indices()
        ),
    });
    return handle;
}

void rendertarget_factory_vulkan::destroy_render_target(handle_holder<rendertarget> &rt) noexcept {
    rendertarget_db.erase(rt.handle());
}

abstract_rendertarget_vulkan &
rendertarget_factory_vulkan::get_render_target_vulkan(handle_holder<rendertarget> handle) {
    return *rendertarget_db.at(handle.handle()).get();
}

BRIGHTCPP_GRAPHICS_VULKAN_END
