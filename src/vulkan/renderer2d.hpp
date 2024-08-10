#pragma once

#include "../interfaces/renderer2d.hpp"
#include "render_target.hpp"
#include "vulkan_common.hpp"
#include <brightcpp/common.hpp>
#include <unordered_map>

BRIGHTCPP_GRAPHICS_VULKAN_START

class renderer2d_vulkan : public renderer2d_backend {
    std::reference_wrapper<const render_target_vulkan> rt;

    vk::Device device;
    vk::UniqueRenderPass renderpass;
    vk::UniqueShaderModule vert_shader;
    vk::UniqueShaderModule frag_shader;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;
    std::vector<vk::UniqueFramebuffer> framebufs;
    vk::UniqueCommandPool draw_cmd_pool;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buf;

    vk::Queue graphics_queue;
    std::vector<vk::UniqueSemaphore> rendered_semaphores;
    vk::Queue presentation_queue;
    std::vector<vk::UniqueFence> rendered_fences;
    uint32_t current_img_index, current_frame_flight_index = 0;

  public:
    renderer2d_vulkan(vk::Device device, const render_target_vulkan &rt, const queue_index_set &queue_indices);

    void render_begin();
    void render_end();
    void draw_texture(handle_holder<image_impl> image, render_texture_info &rect_info) override;
};

class renderer2d_factory_vulkan : public renderer2d_factory_backend {
    vk::Device device;
    queue_index_set queue_indices;
    std::unordered_map<handle_holder<renderer>::handle_value_t, renderer2d_vulkan> renderer_db;

  public:
    renderer2d_factory_vulkan(vk::Device _device, queue_index_set &_queue_indices);
    handle_holder<renderer>::handle_value_t make(handle_holder<render_target> rt_handle) override;
    renderer2d_backend &get(handle_holder<renderer> handle) override;
};

BRIGHTCPP_GRAPHICS_VULKAN_END
