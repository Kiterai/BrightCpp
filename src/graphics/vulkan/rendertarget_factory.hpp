#pragma once

#include "../../entity_holder.hpp"
#include "../../interfaces/rendertarget_factory.hpp"
#include "rendertarget.hpp"
#include "vulkan_common.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

class rendertarget_factory_vulkan : public rendertarget_factory_backend {
    entity_holder<std::unique_ptr<vulkan::abstract_rendertarget_vulkan>> rendertarget_db;

  public:
    ~rendertarget_factory_vulkan() override;

    entity_handle_t create_render_target(window_backend &window) override;
    entity_handle_t create_render_target(handle_holder<image_impl> image) override;
    void destroy_render_target(handle_holder<rendertarget> &rt) noexcept override;
    abstract_rendertarget_vulkan &get_render_target_vulkan(handle_holder<rendertarget> handle);
};

BRIGHTCPP_GRAPHICS_VULKAN_END
