#pragma once

#include "../interfaces/graphics.hpp"
#include "util.hpp"
#include "vma.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct texture_resource : public texture_backend {
  public:
    texture_resource(
        vma::UniqueImage &&image,
        vma::UniqueAllocation &&allocation,
        vk::UniqueImageView &&image_view,
        vk::UniqueDescriptorSet &&desc_set);

    vma::UniqueImage image;
    vma::UniqueAllocation allocation;
    vk::UniqueImageView image_view;
    vk::UniqueDescriptorSet desc_set;
};

class texture_factory : public texture_factory_backend {
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

    std::unique_ptr<texture_backend> make(const uint8_t *data, uint32_t w, uint32_t h) override;
    void destroy(texture_backend &image) noexcept override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
