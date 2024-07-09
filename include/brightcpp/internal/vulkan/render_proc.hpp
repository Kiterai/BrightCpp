#pragma once

#include <brightcpp/internal/vulkan/render_target.hpp>

vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format);
vk::UniqueShaderModule create_vert_shader(vk::Device device);
vk::UniqueShaderModule create_frag_shader(vk::Device device);

vk::UniquePipelineLayout create_pipeline_layout(vk::Device device);
vk::UniquePipeline create_pipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::PipelineLayout pipeline_layout, vk::ShaderModule vert_shader, vk::ShaderModule frag_shader);

class render_proc {
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
    uint32_t current_img_index;

  public:
    render_proc(vk::Device device, const render_target &rt, const queue_index_set &queue_indices);

    void render_begin(const render_target &rt);
    void render_end(const render_target &rt);
    void draw_rect(const render_target &rt, float x, float y, float w, float h, float theta, float ax, float ay);
};
