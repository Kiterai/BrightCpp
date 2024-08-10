#pragma once

#include "../interfaces/graphics.hpp"
#include "render_target.hpp"
#include "renderer2d.hpp"
#include "texture.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/render_target.hpp>
#include <memory>
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

class graphics_vulkan : public graphics_backend {
    std::shared_ptr<os_util_backend> os_util;

    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vma::UniqueAllocator allocator;

    std::vector<vk::SurfaceKHR> surface_needed_support;

    using handle_t = handle_holder<render_target>;
    using handle_value_t = handle_t::handle_value_t;
    std::unordered_map<handle_value_t, vulkan::render_target_vulkan> rendertarget_db;

    std::unique_ptr<texture_factory_backend> tex_factory;
    std::unique_ptr<renderer2d_factory_backend> renderer2d_factory;

  public:
    graphics_vulkan(const std::shared_ptr<os_util_backend> &_os_util);
    ~graphics_vulkan();

    void wait_idle();

    handle_holder<render_target>::handle_value_t create_render_target(window_backend &window) override;
    void destroy_render_target(handle_holder<render_target> &rt) noexcept override;
    render_target_vulkan &get_render_target_vulkan(handle_holder<render_target> handle);
};

std::unique_ptr<graphics_backend> make_graphics_vulkan(const std::shared_ptr<os_util_backend> &os_util);

BRIGHTCPP_GRAPHICS_VULKAN_END
