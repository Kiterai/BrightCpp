#pragma once

#include "rendertarget.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

class window_rendertarget_vulkan : public abstract_rendertarget_vulkan {
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
    window_rendertarget_vulkan(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, const queue_index_set &queue_indices, vk::UniqueSurfaceKHR &&surface);
    window_rendertarget_vulkan(window_rendertarget_vulkan&&) = default;
    window_rendertarget_vulkan& operator=(window_rendertarget_vulkan&&) = default;
    ~window_rendertarget_vulkan() override;

    const std::vector<vk::UniqueImageView> &image_views() const override { return swapchain_imageviews; }
    vk::Format format() const override { return swapchain.format.format; }
    vk::Extent2D extent() const override { return swapchain.extent; }

    render_begin_info render_begin() override;
    void render_end() override;
    void wait_idle() override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
