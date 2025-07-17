#pragma once

#include "../../interfaces/rendertarget_factory.hpp"
#include "rendertarget.hpp"
#include "vulkan_common.hpp"
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

class rendertarget_factory_vulkan : public rendertarget_factory_backend {
    using handle_t = handle_holder<rendertarget>;
    using handle_value_t = handle_t::handle_value_t;
    std::unordered_map<handle_value_t, std::unique_ptr<vulkan::abstract_rendertarget_vulkan>> rendertarget_db;

  public:
    ~rendertarget_factory_vulkan() override;

    handle_holder<rendertarget>::handle_value_t create_render_target(window_backend &window) override;
    handle_holder<rendertarget>::handle_value_t create_render_target(handle_holder<image_impl> image) override;
    void destroy_render_target(handle_holder<rendertarget> &rt) noexcept override;
    abstract_rendertarget_vulkan &get_render_target_vulkan(handle_holder<rendertarget> handle);
};

BRIGHTCPP_GRAPHICS_VULKAN_END
