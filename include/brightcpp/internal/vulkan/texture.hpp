#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/internal/vulkan/vma.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct texture_resource {
    vma::UniqueImage image;
    vma::UniqueAllocation allocation;
    vk::UniqueImageView image_view;
    vk::UniqueDescriptorSet desc_set;
};

texture_resource create_texture(vma::Allocator allocator, const uint8_t *data, uint32_t w, uint32_t h, vk::Queue queue, vk::CommandBuffer cmd_buf);

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
