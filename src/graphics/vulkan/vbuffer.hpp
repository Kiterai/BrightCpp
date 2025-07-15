#pragma once

#include <brightcpp/common.hpp>
#include <unordered_map>

#include "../../global_module.hpp"
#include "../../interfaces/vbuffer_factory.hpp"
#include "util.hpp"
#include "vma.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

struct vbuffer_vulkan {
    vma::UniqueBuffer buffer;
    vma::UniqueAllocation allocation;
};

class vbuffer_factory_vulkan : public vbuffer_factory_backend {
    vk::Device device;
    vma::Allocator allocator;

  public:
    vbuffer_factory_vulkan();
    ~vbuffer_factory_vulkan();

    handle_holder<vbuffer_impl>::handle_value_t make(const uint8_t *data, size_t bytes_num) override;
    void destroy(const handle_holder<vbuffer_impl> &) noexcept override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
