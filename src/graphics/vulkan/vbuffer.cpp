#pragma once

#include "vbuffer.hpp"
#include "../../global_module.hpp"
#include "graphics.hpp"
#include "util.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

vbuffer_factory_vulkan::vbuffer_factory_vulkan()
    : device{global_module<vulkan::graphics_vulkan>::get().get_device()},
      allocator{global_module<vulkan::graphics_vulkan>::get().get_allocator()} {}
vbuffer_factory_vulkan::~vbuffer_factory_vulkan() = default;

handle_holder<vbuffer_impl>::handle_value_t vbuffer_factory_vulkan::make(const uint8_t *data, size_t bytes_num) {

    return 0;
}
void vbuffer_factory_vulkan::destroy(const handle_holder<vbuffer_impl> &) noexcept {
    // TODO
};

BRIGHTCPP_GRAPHICS_VULKAN_END
