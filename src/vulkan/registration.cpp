#pragma once

#include "registration.hpp"
#include "../global_module.hpp"
#include "graphics.hpp"
#include "renderer2d.hpp"
#include "texture.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

struct vulkan_object_container : public object_container {
    ~vulkan_object_container() = default;

    std::unique_ptr<graphics_vulkan> graphics;
    std::unique_ptr<texture_factory_vulkan> tex_factory;
    std::unique_ptr<renderer2d_factory_vulkan> renderer2d_factory;
};

std::unique_ptr<object_container> register_objects() {
    auto c = std::make_unique<vulkan_object_container>();

    c->graphics = std::make_unique<graphics_vulkan>();
    global_module<graphics_backend>::set(*c->graphics.get());
    global_module<graphics_vulkan>::set(*c->graphics.get());

    c->tex_factory = std::make_unique<texture_factory_vulkan>();
    global_module<texture_factory_backend>::set(*c->tex_factory.get());
    global_module<texture_factory_vulkan>::set(*c->tex_factory.get());

    c->renderer2d_factory = std::make_unique<renderer2d_factory_vulkan>();
    global_module<renderer2d_factory_backend>::set(*c->renderer2d_factory.get());
    global_module<renderer2d_factory_vulkan>::set(*c->renderer2d_factory.get());

    return c;
}

BRIGHTCPP_GRAPHICS_VULKAN_END
