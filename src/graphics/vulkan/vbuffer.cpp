#pragma once

#include "vbuffer.hpp"
#include "util.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

vbuffer_factory_vulkan::vbuffer_factory_vulkan() {
    // TODO
}
vbuffer_factory_vulkan::~vbuffer_factory_vulkan() = default;

handle_holder<vbuffer_impl>::handle_value_t vbuffer_factory_vulkan::make(const uint8_t *data,
                                                                         size_t bytes_num) {
    // TODO
    return 0;
}
void vbuffer_factory_vulkan::destroy(const handle_holder<vbuffer_impl> &) noexcept {
    // TODO
};

BRIGHTCPP_GRAPHICS_VULKAN_END
