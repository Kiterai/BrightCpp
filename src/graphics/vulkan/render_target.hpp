#pragma once

#include "../../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include "util.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

constexpr uint32_t frames_inflight = 2;

struct render_begin_info {
  vk::CommandBuffer cmd_buf;
  uint32_t img_index;
};

class abstract_render_target_vulkan {
  public:
    virtual const std::vector<vk::UniqueImageView> &image_views() const = 0;
    virtual vk::Format format() const = 0;
    virtual vk::Extent2D extent() const = 0;

    virtual render_begin_info render_begin() = 0;
    virtual void render_end() = 0;
    virtual void wait_idle() = 0;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
