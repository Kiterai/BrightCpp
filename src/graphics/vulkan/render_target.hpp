#pragma once

#include "../../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include "util.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct render_begin_info {
  vk::CommandBuffer cmd_buf;
  uint32_t img_index;
};

class abstract_render_target_vulkan {
  public:
    virtual const std::vector<vk::UniqueImageView> &image_views() const = 0;
    virtual vk::Format format() const = 0;
    virtual vk::Extent2D extent() const = 0;

    virtual render_begin_info render_begin() = 0;
    virtual void render_end() = 0;
    virtual void wait_idle() = 0;
};

class window_render_target_vulkan : public abstract_render_target_vulkan {
    vk::Device device;
    vk::UniqueSurfaceKHR surface;
    SwapchainWithInfo swapchain;
    std::vector<vk::Image> swapchain_images;
    std::vector<vk::UniqueImageView> swapchain_imageviews;
    std::vector<vk::UniqueSemaphore> image_acquire_semaphores;

    vk::UniqueCommandPool draw_cmd_pool;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buf;

    vk::Queue graphics_queue, presentation_queue;
    std::vector<vk::UniqueSemaphore> rendered_semaphores;
    std::vector<vk::UniqueFence> rendered_fences;
    uint32_t current_img_index, current_frame_flight_index = 0;

    bool rendering = false;

  public:
    // this handles ownership of surface
    window_render_target_vulkan(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, const queue_index_set &queue_indices, vk::UniqueSurfaceKHR &&surface);
    window_render_target_vulkan(window_render_target_vulkan&&) = default;
    window_render_target_vulkan& operator=(window_render_target_vulkan&&) = default;
    ~window_render_target_vulkan();

    const std::vector<vk::UniqueImageView> &image_views() const override { return swapchain_imageviews; }
    vk::Format format() const override { return swapchain.format.format; }
    vk::Extent2D extent() const override { return swapchain.extent; }

    render_begin_info render_begin() override;
    void render_end() override;
    void wait_idle() override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
