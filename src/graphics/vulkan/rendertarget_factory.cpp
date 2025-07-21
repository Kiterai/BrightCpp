#pragma once

#include "rendertarget_factory.hpp"
#include "../../global_module.hpp"
#include "graphics.hpp"
#include "texture_rendertarget.hpp"
#include "window_rendertarget.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

rendertarget_factory_vulkan::~rendertarget_factory_vulkan() { global_module<graphics_vulkan>::get().wait_idle(); }

entity_handle_t rendertarget_factory_vulkan::create_render_target(window_backend &window) {
    const auto &g = global_module<graphics_vulkan>::get();

    return rendertarget_db.make(std::make_unique<window_rendertarget_vulkan>(
        g.get_instance(), g.get_physdevice(), g.get_device(), g.get_queue_indices(),
        window.get_vulkan_surface(g.get_instance())));
}

entity_handle_t rendertarget_factory_vulkan::create_render_target(handle_holder<image_impl> image) {
    const auto &g = global_module<graphics_vulkan>::get();

    const auto &tex = global_module<texture_factory_vulkan>::get().get(image);

    return rendertarget_db.make(
        std::make_unique<texture_rendertarget_vulkan>(g.get_device(), tex.format, vk::Extent2D(tex.w_int, tex.h_int),
                                                      std::vector<vk::Image>{tex.image.get()}, g.get_queue_indices()));
}

void rendertarget_factory_vulkan::destroy_render_target(handle_holder<rendertarget> &rt) noexcept {
    rendertarget_db.destroy(rt.handle());
}

abstract_rendertarget_vulkan &
rendertarget_factory_vulkan::get_render_target_vulkan(handle_holder<rendertarget> handle) {
    return *rendertarget_db.get(handle.handle()).get();
}

BRIGHTCPP_GRAPHICS_VULKAN_END
