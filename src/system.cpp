#include "system.hpp"
#include <iostream>
#include <optional>

#include "glfw/glfw.hpp"
#include "vulkan/graphics.hpp"

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

static size_t initializer_count = 0;

struct global_objects_t {
    std::shared_ptr<os_util_backend> os_util;
    std::shared_ptr<graphics_backend> graphics;
    std::shared_ptr<texture_factory_backend> tex_factory;
};

std::optional<global_objects_t> global_objects;

system_initializer::system_initializer() {
    if (initializer_count == 0) {
        global_objects.emplace();

        global_objects->os_util = glfw::make_glfw_manager();
        global_module<os_util_backend>::set(*global_objects->os_util);

        global_objects->graphics = vulkan::make_graphics_vulkan(global_objects->os_util);
        global_module<graphics_backend>::set(*global_objects->graphics);
        
        global_objects->tex_factory = global_module<graphics_backend>::get().create_texture_factory();
        global_module<texture_factory_backend>::set(*global_objects->tex_factory);

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
        global_objects.reset();
    }
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
