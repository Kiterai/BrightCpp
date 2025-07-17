#include "texture_rendertarget.hpp"
#include "util.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

texture_rendertarget_vulkan::texture_rendertarget_vulkan(vk::Device device, vk::Format _format, vk::Extent2D _extent, std::vector<vk::Image> &&images, const queue_index_set &queue_indices)
    : device{device},
      _format{_format},
      _extent{_extent},
      images{images},
      imageviews{create_image_views(device, images, _format)},
      draw_cmd_pool{create_cmd_pool(device, queue_indices, vk::CommandPoolCreateFlagBits::eResetCommandBuffer)},
      draw_cmd_buf{create_cmd_bufs(device, draw_cmd_pool.get(), 1)},
      graphics_queue{device.getQueue(queue_indices.graphics_queue, 0)},
      rendered_fences{create_fences(device, true, 1)} {}

texture_rendertarget_vulkan::~texture_rendertarget_vulkan() {
    if (rendering)
        render_end();
    wait_idle();
}

vk::ImageLayout texture_rendertarget_vulkan::srcLayout() const {
    return vk::ImageLayout::eShaderReadOnlyOptimal;
}
vk::ImageLayout texture_rendertarget_vulkan::dstLayout() const {
    return vk::ImageLayout::eShaderReadOnlyOptimal;
}

render_begin_info texture_rendertarget_vulkan::render_begin() {
    assert(!rendering);
    rendering = true;

    device.waitForFences({rendered_fences[current_frame_flight_index].get()}, VK_TRUE, UINT64_MAX);

    const auto &cmd_buf = draw_cmd_buf[current_frame_flight_index].get();
    const auto &fence = rendered_fences[current_frame_flight_index].get();

    cmd_buf.reset();
    device.resetFences({fence});

    vk::CommandBufferBeginInfo cmdBeginInfo;
    cmd_buf.begin(cmdBeginInfo);

    return render_begin_info{
        .cmd_buf = cmd_buf,
        .img_index = current_frame_flight_index,
    };
}

void texture_rendertarget_vulkan::render_end() {
    assert(rendering);
    rendering = false;

    const auto &cmd_buf = draw_cmd_buf[current_frame_flight_index].get();

    cmd_buf.end();

    {
        vk::SubmitInfo submitInfo;

        auto submit_cmd_buf = {cmd_buf};
        submitInfo.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submitInfo.pCommandBuffers = submit_cmd_buf.begin();

        graphics_queue.submit({submitInfo}, rendered_fences[current_frame_flight_index].get());
    }
}

void texture_rendertarget_vulkan::wait_idle() {
    if (rendered_fences.empty())
        return;
    std::array<vk::Fence, 1> fences;
    for (uint32_t i = 0; i < rendered_fences.size(); i++)
        fences[i] = rendered_fences[i].get();
    device.waitForFences(fences, VK_TRUE, UINT64_MAX);
}

BRIGHTCPP_GRAPHICS_VULKAN_END
