#pragma once

#include "../interfaces/graphics.hpp"
#include "vulkan_common.hpp"
#include "render_target.hpp"
#include <brightcpp/common.hpp>
#include <brightcpp/render_target.hpp>
#include <memory>

BRIGHTCPP_GRAPHICS_VULKAN_START

std::unique_ptr<graphics_backend> make_graphics_vulkan(const std::shared_ptr<os_util_backend> &os_util);
render_target_vulkan &get_render_target_vulkan(handle_holder<render_target> handle);

BRIGHTCPP_GRAPHICS_VULKAN_END
