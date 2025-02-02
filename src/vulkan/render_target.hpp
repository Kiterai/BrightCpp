#pragma once

#include "../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include "util.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct render_begin_info {
  vk::CommandBuffer cmd_buf;
  uint32_t img_index;
};

class render_target_vulkan {
    vk::UniqueSurfaceKHR surface;
    SwapchainWithInfo swapchain;
    std::vector<vk::Image> swapchain_images;
    std::vector<vk::UniqueImageView> swapchain_imageviews;
    vk::UniqueSemaphore image_acquire_semaphore;

    vk::UniqueCommandPool draw_cmd_pool;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buf;

    vk::Queue graphics_queue, presentation_queue;
    std::vector<vk::UniqueSemaphore> rendered_semaphores;
    std::vector<vk::UniqueFence> rendered_fences;
    uint32_t current_img_index, current_frame_flight_index = 0;

  public:
    // this handles ownership of surface
    render_target_vulkan(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, const queue_index_set &queue_indices, vk::UniqueSurfaceKHR &&surface);

    const auto &image_views() const { return swapchain_imageviews; }
    auto format() const { return swapchain.format.format; }
    auto extent() const { return swapchain.extent; }

    render_begin_info render_begin(vk::Device device);
    void render_end();
};

BRIGHTCPP_GRAPHICS_VULKAN_END
