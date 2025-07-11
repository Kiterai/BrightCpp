#pragma once

#include "../../interfaces/graphics.hpp"
#include "render_target.hpp"
#include "renderer2d.hpp"
#include "texture.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/render_target.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

class graphics_vulkan : public graphics_backend {
    std::reference_wrapper<os_util_backend> os_util;

    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vma::UniqueAllocator allocator;

    using handle_t = handle_holder<render_target>;
    using handle_value_t = handle_t::handle_value_t;
    std::unordered_map<handle_value_t, vulkan::render_target_vulkan> rendertarget_db;

  public:
    graphics_vulkan();
    ~graphics_vulkan();

    void wait_idle();

    handle_holder<render_target>::handle_value_t create_render_target(window_backend &window) override;
    void destroy_render_target(handle_holder<render_target> &rt) noexcept override;

    vk::Device get_device() const { return device.get(); }
    vma::Allocator get_allocator() const { return allocator.get(); }
    const queue_index_set &get_queue_indices() const { return queue_indices; }

    render_target_vulkan &get_render_target_vulkan(handle_holder<render_target> handle);
};

BRIGHTCPP_GRAPHICS_VULKAN_END
