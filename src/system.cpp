#include <brightcpp/internal/glfw.hpp>
#include <brightcpp/internal/system.hpp>
#include <brightcpp/internal/vulkan.hpp>
#include <iostream>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct {
    std::unique_ptr<system_module> glfw_initializer;
    std::unique_ptr<system_module> vulkan_manager;
} system_modules;
static size_t initializer_count = 0;

system_initializer::system_initializer() {
    if (initializer_count == 0) {
        system_modules.glfw_initializer = make_glfw_initializer();
        system_modules.vulkan_manager = make_vulkan_manager();
#ifdef _DEBUG
        std::cout << "successfully initialized BrightCpp." << std::endl;
#endif
    }
    initializer_count++;
}

system_initializer::~system_initializer() {
    initializer_count--;
    if (initializer_count == 0) {
#ifdef _DEBUG
        std::cout << "shutdown BrightCpp..." << std::endl;
#endif
        system_modules.vulkan_manager.reset();
        system_modules.glfw_initializer.reset();
    }
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
