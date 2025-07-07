#pragma once

#include "vulkan_common.hpp"
#include <brightcpp/common.hpp>
#include <span>
#include <vector>
#include <vulkan/vulkan.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct queue_index_set {
    uint32_t graphics_queue, presentation_queue;
};

vk::UniqueCommandPool create_cmd_pool(vk::Device device, const queue_index_set &queue_indices, vk::CommandPoolCreateFlags flags);
std::vector<vk::UniqueCommandBuffer> create_cmd_bufs(vk::Device device, vk::CommandPool pool, uint32_t num);

struct SwapchainWithInfo {
    vk::SurfaceFormatKHR format;
    vk::Extent2D extent;
    vk::UniqueSwapchainKHR swapchain;
};

SwapchainWithInfo create_swapchain(vk::Device device, vk::PhysicalDevice phys_device, vk::SurfaceKHR surface);

vk::UniqueImageView create_image_view(vk::Device device, vk::Image image, vk::Format format);
std::vector<vk::UniqueImageView> create_image_views(vk::Device device, std::span<const vk::Image> images, vk::Format format);

vk::UniqueFramebuffer create_frame_buf(vk::Device device, vk::ImageView image_view, const vk::Extent2D &extent, vk::RenderPass renderpass);
std::vector<vk::UniqueFramebuffer> create_frame_bufs(vk::Device device, std::span<const vk::UniqueImageView> image_views, const vk::Extent2D &extent, vk::RenderPass renderpass);

vk::UniqueFence create_fence(vk::Device device, bool signaled);
std::vector<vk::UniqueFence> create_fences(vk::Device device, bool signaled, uint32_t num);

vk::UniqueSemaphore create_semaphore(vk::Device device);
std::vector<vk::UniqueSemaphore> create_semaphores(vk::Device device, uint32_t num);

BRIGHTCPP_GRAPHICS_VULKAN_END