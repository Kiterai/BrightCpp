#include "render_target.hpp"

BRIGHTCPP_GRAPHICS_VULKAN_START

render_target::render_target(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, vk::UniqueSurfaceKHR &&_surface)
    : surface{std::move(_surface)},
      swapchain{create_swapchain(device, phys_device, surface.get())},
      swapchain_images{device.getSwapchainImagesKHR(swapchain.swapchain.get())},
      swapchain_imageviews{create_image_views(device, swapchain_images, swapchain.format.format)},
      image_acquire_semaphore{create_semaphore(device)} {}

[[noreturn]] render_target::render_target() {
    throw std::runtime_error("empty render target created");
}

uint32_t render_target::acquire_frame(vk::Device device) const {
    vk::ResultValue result = device.acquireNextImageKHR(swapchain.swapchain.get(), 1'000'000'000, image_acquire_semaphore.get(), {});
    if (result.result != vk::Result::eSuccess) {
        throw std::runtime_error("error occured on acquireNextImageKHR(): " + vk::to_string(result.result));
    }
    return result.value;
}
void render_target::present(vk::Queue presentation_queue, uint32_t img_index, std::span<const vk::Semaphore> wait_semaphore) const {
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
