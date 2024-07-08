#pragma once

#include <brightcpp/internal/system.hpp>
#include <memory>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

void setup_vulkan_manager();
void shutdown_vulkan_manager();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
