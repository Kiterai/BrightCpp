#include "render_target.hpp"
#include "util.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

constexpr uint32_t frames_inflight = 2;

static auto create_draw_cmd_pool(vk::Device device, const queue_index_set &queue_indices) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

render_target_vulkan::render_target_vulkan(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, const queue_index_set &queue_indices, vk::UniqueSurfaceKHR &&_surface)
    : surface{std::move(_surface)},
      swapchain{create_swapchain(device, phys_device, surface.get())},
      swapchain_images{device.getSwapchainImagesKHR(swapchain.swapchain.get())},
      swapchain_imageviews{create_image_views(device, swapchain_images, swapchain.format.format)},
      image_acquire_semaphore{create_semaphore(device)},
      draw_cmd_pool{create_draw_cmd_pool(device, queue_indices)},
      draw_cmd_buf{create_cmd_bufs(device, draw_cmd_pool.get(), uint32_t(swapchain_images.size()))},
      graphics_queue{device.getQueue(queue_indices.graphics_queue, 0)},
      presentation_queue{device.getQueue(queue_indices.presentation_queue, 0)},
      rendered_semaphores{create_semaphores(device, uint32_t(swapchain_images.size()))},
      rendered_fences{create_fences(device, true, frames_inflight)} {}

render_begin_info render_target_vulkan::render_begin(vk::Device device) {
    device.waitForFences({rendered_fences[current_frame_flight_index].get()}, VK_TRUE, UINT64_MAX);
    current_frame_flight_index++;
    current_frame_flight_index %= frames_inflight;

    current_img_index = acquire_frame(device);
    const auto &cmd_buf = draw_cmd_buf[current_img_index].get();
    const auto &fence = rendered_fences[current_frame_flight_index].get();

    cmd_buf.reset();
    device.resetFences({fence});

    vk::CommandBufferBeginInfo cmdBeginInfo;
    cmd_buf.begin(cmdBeginInfo);

    return render_begin_info{
        .cmd_buf = cmd_buf,
        .img_index = current_img_index,
    };
}

void render_target_vulkan::render_end() {
    const auto &cmd_buf = draw_cmd_buf[current_img_index].get();
    const auto &rendered_semaphore = rendered_semaphores[current_img_index].get();

    cmd_buf.end();

    {
        vk::SubmitInfo submitInfo;

        auto submit_cmd_buf = {cmd_buf};
        submitInfo.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submitInfo.pCommandBuffers = submit_cmd_buf.begin();

        auto render_signal_semaphores = {rendered_semaphore};
        submitInfo.signalSemaphoreCount = uint32_t(render_signal_semaphores.size());
        submitInfo.pSignalSemaphores = render_signal_semaphores.begin();

        vk::Semaphore renderwaitSemaphores[] = {image_prepared_semaphore()};
        vk::PipelineStageFlags renderwaitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = renderwaitSemaphores;
        submitInfo.pWaitDstStageMask = renderwaitStages;

        graphics_queue.submit({submitInfo}, rendered_fences[current_frame_flight_index].get());
    }

    present(current_img_index, std::array{rendered_semaphore});
}

uint32_t render_target_vulkan::acquire_frame(vk::Device device) const {
    vk::ResultValue result = device.acquireNextImageKHR(swapchain.swapchain.get(), 1'000'000'000, image_acquire_semaphore.get(), {});
    if (result.result != vk::Result::eSuccess) {
        throw std::runtime_error("error occured on acquireNextImageKHR(): " + vk::to_string(result.result));
    }
    return result.value;
}
void render_target_vulkan::present(uint32_t img_index, std::span<const vk::Semaphore> wait_semaphore) const {
    vk::PresentInfoKHR presentInfo;

    auto presentSwapchains = {swapchain.swapchain.get()};
    auto imgIndices = {img_index};

    presentInfo.swapchainCount = uint32_t(presentSwapchains.size());
    presentInfo.pSwapchains = presentSwapchains.begin();
    presentInfo.pImageIndices = imgIndices.begin();

    presentInfo.waitSemaphoreCount = uint32_t(wait_semaphore.size());
    presentInfo.pWaitSemaphores = wait_semaphore.data();

    auto result = presentation_queue.presentKHR(presentInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("error occured on presentKHR(): " + vk::to_string(result));
    }
}

BRIGHTCPP_GRAPHICS_VULKAN_END
