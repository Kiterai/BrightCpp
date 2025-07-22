#pragma once

#include "../../interfaces/renderer2d.hpp"
#include "rendertarget.hpp"
#include "vulkan_common.hpp"
#include "window_rendertarget.hpp"
#include <brightcpp/common.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

class renderer2d_vulkan : public renderer2d_backend {
    std::reference_wrapper<abstract_rendertarget_vulkan> rt;

    vk::Device device;
    vk::UniqueRenderPass renderpass;
    vk::UniqueShaderModule vert_shader;
    vk::UniqueShaderModule frag_shader;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;
    std::vector<vk::UniqueFramebuffer> framebufs;

    vk::CommandBuffer cmd_buf;

    handle_holder<image_impl>::handle_value_t last_binded_texture;
    bool rendering = false;

  public:
    renderer2d_vulkan(vk::Device device, abstract_rendertarget_vulkan &rt, const queue_index_set &queue_indices);
    ~renderer2d_vulkan() override;

    void render_begin() override;
    void render_end() override;

    void attach_texture(handle_holder<image_impl> image) override;
    void draw_polygon(size_t num, handle_holder<vbuffer_impl> vbuffer, const renderer2d_uniform &uniform) override;
};

class renderer2d_factory_vulkan : public renderer2d_factory_backend {
    vk::Device device;
    queue_index_set queue_indices;

  public:
    renderer2d_factory_vulkan();
    std::unique_ptr<renderer2d_backend> make(rendertarget rt_handle) override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
