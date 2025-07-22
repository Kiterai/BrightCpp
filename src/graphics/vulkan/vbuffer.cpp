#pragma once

#include "vbuffer.hpp"
#include "../../global_module.hpp"
#include "graphics.hpp"
#include "util.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

using g_graphics = global_module<vulkan::graphics_vulkan>;

vbuffer_factory_vulkan::vbuffer_factory_vulkan()
    : device{g_graphics::get().get_device()}, allocator{g_graphics::get().get_allocator()} {}
vbuffer_factory_vulkan::~vbuffer_factory_vulkan() = default;

entity_handle_t vbuffer_factory_vulkan::make(size_t bytes_num) {
    auto [buf, buf_allocation] = create_empty_buffer(allocator, bytes_num, vk::BufferUsageFlagBits::eVertexBuffer);

    const auto handle_value = vbuffer_db.size();

    vbuffer_db.insert({
        handle_value,
        vbuffer_vulkan{
            .buffer = std::move(buf),
            .allocation = std::move(buf_allocation),
        },
    });

    return 0;
}

void vbuffer_factory_vulkan::update_data(entity_handle_t handle, const uint8_t *data,
                                         size_t bytes_num) {
    auto [tmpbuf, tmpbuf_allocation] =
        create_filled_buffer(allocator, data, bytes_num, vk::BufferUsageFlagBits::eTransferSrc);

    auto cmdbuf = g_graphics::get().begin_onetime_command();
    cmdbuf.copyBuffer(tmpbuf.get(), vbuffer_db.at(handle).buffer.get(), {vk::BufferCopy{0, 0, bytes_num}});
    auto fence = g_graphics::get().flush_onetime_command();

    device.waitForFences({fence}, true, UINT64_MAX);
}

void vbuffer_factory_vulkan::destroy(entity_handle_t handle) noexcept {
    vbuffer_db.erase(handle);
};

const vbuffer_vulkan &vbuffer_factory_vulkan::get_vbuffer(entity_handle_t handle) const {
    return vbuffer_db.at(handle);
}

BRIGHTCPP_GRAPHICS_VULKAN_END
