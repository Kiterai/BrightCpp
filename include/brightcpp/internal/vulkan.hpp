#pragma once

#include <GLFW/glfw3.h>
#include <brightcpp/common.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

void setup_vulkan_manager();
void shutdown_vulkan_manager();

void create_render_target(GLFWwindow *window);
void destroy_render_target(GLFWwindow *window);
void set_current_render_target(GLFWwindow *window);
void apply_render();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
