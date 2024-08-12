#pragma once

#include "../interfaces/texture_factory.hpp"
#include "util.hpp"
#include "vma.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/image_decl.hpp>
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

struct texture_vulkan {
  public:
    vma::UniqueImage image;
    vma::UniqueAllocation allocation;
    vk::UniqueImageView image_view;
    vk::UniqueDescriptorSet desc_set;
};

class texture_factory_vulkan : public texture_factory_backend {
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

    using handle_t = handle_holder<image_impl>;
    using handle_value_t = handle_t::handle_value_t;
    std::unordered_map<handle_value_t, texture_vulkan> texture_db;

  public:
    texture_factory_vulkan();

    handle_holder<image_impl>::handle_value_t make(const uint8_t *data, uint32_t w, uint32_t h) override;
    void destroy(handle_holder<image_impl> &image) noexcept override;

    const texture_vulkan &get(handle_holder<image_impl> &image) const;

    vk::DescriptorSetLayout get_descriptor_set_layout() const;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
