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

    using handle_t = handle_holder<vbuffer_impl>::handle_value_t;
    std::unordered_map<handle_t, vbuffer_vulkan> vbuffer_db;

  public:
    vbuffer_factory_vulkan();
    ~vbuffer_factory_vulkan();

    entity_handle_t make(size_t bytes_num) override;
    void update_data(entity_handle_t, const uint8_t *data, size_t bytes_num) override;
    void destroy(entity_handle_t) noexcept override;

    const vbuffer_vulkan &get_vbuffer(entity_handle_t vbuffer) const;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
