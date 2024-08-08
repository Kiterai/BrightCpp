#pragma once

#include "vma.hpp"
#include "vulkan.hpp"
#include "vulkan_util.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct texture_resource {
    vma::UniqueImage image;
    vma::UniqueAllocation allocation;
    vk::UniqueImageView image_view;
    vk::UniqueDescriptorSet desc_set;
};

class texture_factory {
    vk::Device device;
    vma::Allocator allocator;
    vk::UniqueSampler sampler;
    vk::UniqueDescriptorSetLayout desc_layout;
    vk::UniqueDescriptorPool desc_pool;
    vk::Queue queue;
    vk::UniqueCommandPool cmd_pool;
    vk::UniqueCommandBuffer cmd_buf;
    vk::UniqueFence texture_creating_fence;

    vk::UniqueDescriptorSet create_texture_descriptor_set(vk::ImageView image_view);

  public:
    texture_factory(vk::Device device, vma::Allocator allocator, const queue_index_set &queue_indices);
    texture_resource create_texture(const uint8_t *data, uint32_t w, uint32_t h);
};

BRIGHTCPP_GRAPHICS_VULKAN_END
