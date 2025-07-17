#pragma once

#include "rendertarget.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

class texture_rendertarget_vulkan : public abstract_rendertarget_vulkan {
    vk::Device device;
    vk::Format _format;
    vk::Extent2D _extent;
    std::vector<vk::Image> images;
    std::vector<vk::UniqueImageView> imageviews;

    vk::UniqueCommandPool draw_cmd_pool;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buf;

    vk::Queue graphics_queue;
    // std::vector<vk::UniqueSemaphore> rendered_semaphores;
    std::vector<vk::UniqueFence> rendered_fences;
    uint32_t current_img_index, current_frame_flight_index = 0;

    bool rendering = false;

  public:
    texture_rendertarget_vulkan(vk::Device device, vk::Format _format, vk::Extent2D _extent, std::vector<vk::Image> &&images, const queue_index_set &queue_indices);
    texture_rendertarget_vulkan(texture_rendertarget_vulkan&&) = default;
    texture_rendertarget_vulkan& operator=(texture_rendertarget_vulkan&&) = default;
    ~texture_rendertarget_vulkan() override;

    const std::vector<vk::UniqueImageView> &image_views() const override { return imageviews; }
    vk::Format format() const override { return _format; }
    vk::Extent2D extent() const override { return _extent; }

    render_begin_info render_begin() override;
    void render_end() override;
    void wait_idle() override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
