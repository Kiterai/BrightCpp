#pragma once

#include "../../interfaces/graphics.hpp"
#include "rendertarget.hpp"
#include "renderer2d.hpp"
#include "texture.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/rendertarget.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

class graphics_vulkan : public graphics_backend {
    std::reference_wrapper<os_util_backend> os_util;

    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vma::UniqueAllocator allocator;

    uint32_t onetime_cmdbuf_index;
    vk::UniqueCommandPool onetime_cmdpool;
    std::vector<vk::UniqueCommandBuffer> onetime_cmdbufs;
    std::vector<vk::UniqueFence> onetime_cmd_fences;

  public:
    graphics_vulkan();
    ~graphics_vulkan();

    void wait_idle();

    vk::Instance get_instance() const { return instance.get(); }
    vk::PhysicalDevice get_physdevice() const { return phys_device; }
    vk::Device get_device() const { return device.get(); }
    vma::Allocator get_allocator() const { return allocator.get(); }
    const queue_index_set &get_queue_indices() const { return queue_indices; }

    vk::CommandBuffer begin_onetime_command();
    vk::Fence flush_onetime_command();
};

BRIGHTCPP_GRAPHICS_VULKAN_END
