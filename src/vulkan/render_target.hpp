#pragma once

#include "../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include "util.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

class render_target_vulkan : public render_target_backend {
    vk::UniqueSurfaceKHR surface;
    SwapchainWithInfo swapchain;
    std::vector<vk::Image> swapchain_images;
    std::vector<vk::UniqueImageView> swapchain_imageviews;
    vk::UniqueSemaphore image_acquire_semaphore;

  public:
    // this handles ownership of surface
    render_target_vulkan(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, vk::UniqueSurfaceKHR &&surface);

    const auto &image_views() const { return swapchain_imageviews; }
    auto format() const { return swapchain.format.format; }
    auto extent() const { return swapchain.extent; }
    auto image_prepared_semaphore() const { return image_acquire_semaphore.get(); }

    uint32_t acquire_frame(vk::Device device) const;
    void present(vk::Queue presentation_queue, uint32_t img_index, std::span<const vk::Semaphore> wait_semaphore) const;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
