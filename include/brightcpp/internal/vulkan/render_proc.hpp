#pragma once

#include <brightcpp/common.hpp>
#include <brightcpp/internal/linear_algebra.hpp>
#include <brightcpp/internal/vulkan/render_target.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

struct render_rect_info {
    vec2 pos, size, apos, clip_pos, clip_size;
    vec4 color;
    float theta;
};

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
    uint32_t current_img_index, current_frame_flight_index = 0;

  public:
    render_proc(vk::Device device, const render_target &rt, const queue_index_set &queue_indices);

    void render_begin(const render_target &rt);
    void render_end(const render_target &rt);
    void draw_rect(const render_target &rt, render_rect_info rect_info);
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
