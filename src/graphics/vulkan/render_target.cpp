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
    : device{device},
      surface{std::move(_surface)},
      swapchain{create_swapchain(device, phys_device, surface.get())},
      swapchain_images{device.getSwapchainImagesKHR(swapchain.swapchain.get())},
      swapchain_imageviews{create_image_views(device, swapchain_images, swapchain.format.format)},
      image_acquire_semaphores{create_semaphores(device, frames_inflight)},
      draw_cmd_pool{create_draw_cmd_pool(device, queue_indices)},
      draw_cmd_buf{create_cmd_bufs(device, draw_cmd_pool.get(), frames_inflight)},
      graphics_queue{device.getQueue(queue_indices.graphics_queue, 0)},
      presentation_queue{device.getQueue(queue_indices.presentation_queue, 0)},
      rendered_semaphores{create_semaphores(device, frames_inflight)},
      rendered_fences{create_fences(device, true, frames_inflight)} {}

render_target_vulkan::~render_target_vulkan() {
    if (rendering)
        render_end();
    wait_idle();
}

render_begin_info render_target_vulkan::render_begin() {
    assert(!rendering);
    rendering = true;

    device.waitForFences({rendered_fences[current_frame_flight_index].get()}, VK_TRUE, UINT64_MAX);

    vk::ResultValue acquire_image_result = device.acquireNextImageKHR(swapchain.swapchain.get(), 1'000'000'000, image_acquire_semaphores[current_frame_flight_index].get(), {});
    if (acquire_image_result.result != vk::Result::eSuccess) {
        throw std::runtime_error("error occured on acquireNextImageKHR(): " + vk::to_string(acquire_image_result.result));
    }
    current_img_index = acquire_image_result.value;

    const auto &cmd_buf = draw_cmd_buf[current_frame_flight_index].get();
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
    assert(rendering);
    rendering = false;

    const auto &cmd_buf = draw_cmd_buf[current_frame_flight_index].get();
    const auto &rendered_semaphore = rendered_semaphores[current_frame_flight_index].get();

    cmd_buf.end();

    {
        vk::SubmitInfo submitInfo;

        auto submit_cmd_buf = {cmd_buf};
        submitInfo.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submitInfo.pCommandBuffers = submit_cmd_buf.begin();

        auto render_signal_semaphores = {rendered_semaphore};
        submitInfo.signalSemaphoreCount = uint32_t(render_signal_semaphores.size());
        submitInfo.pSignalSemaphores = render_signal_semaphores.begin();

        vk::Semaphore renderwaitSemaphores[] = {image_acquire_semaphores[current_frame_flight_index].get()};
        vk::PipelineStageFlags renderwaitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = renderwaitSemaphores;
        submitInfo.pWaitDstStageMask = renderwaitStages;

        graphics_queue.submit({submitInfo}, rendered_fences[current_frame_flight_index].get());
    }

    // presentation
    vk::PresentInfoKHR presentInfo;

    auto presentSwapchains = {swapchain.swapchain.get()};
    auto imgIndices = {current_img_index};

    presentInfo.swapchainCount = uint32_t(presentSwapchains.size());
    presentInfo.pSwapchains = presentSwapchains.begin();
    presentInfo.pImageIndices = imgIndices.begin();

    const auto wait_semaphore = {rendered_semaphore};
    presentInfo.waitSemaphoreCount = uint32_t(wait_semaphore.size());
    presentInfo.pWaitSemaphores = wait_semaphore.begin();

    auto result = presentation_queue.presentKHR(presentInfo);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("error occured on presentKHR(): " + vk::to_string(result));
    }

    current_frame_flight_index++;
    current_frame_flight_index %= frames_inflight;
}

void render_target_vulkan::wait_idle() {
    if (rendered_fences.empty())
        return;
    std::array<vk::Fence, frames_inflight> fences;
    for (uint32_t i = 0; i < rendered_fences.size(); i++)
        fences[i] = rendered_fences[i].get();
    device.waitForFences(fences, VK_TRUE, UINT64_MAX);
}

BRIGHTCPP_GRAPHICS_VULKAN_END
