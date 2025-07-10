#include "util.hpp"
#include <map>

BRIGHTCPP_GRAPHICS_VULKAN_START

vk::UniqueCommandPool create_cmd_pool(vk::Device device, const queue_index_set &queue_indices, vk::CommandPoolCreateFlags flags) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = flags;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

std::vector<vk::UniqueCommandBuffer> create_cmd_bufs(vk::Device device, vk::CommandPool pool, uint32_t num) {
    vk::CommandBufferAllocateInfo alloc_info;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = num;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;

    return device.allocateCommandBuffersUnique(alloc_info);
}

SwapchainWithInfo create_swapchain(vk::Device device, vk::PhysicalDevice phys_device, vk::SurfaceKHR surface) {
    auto surfaceCapabilities = phys_device.getSurfaceCapabilitiesKHR(surface);
    auto surfaceFormats = phys_device.getSurfaceFormatsKHR(surface);
    auto surfacePresentModes = phys_device.getSurfacePresentModesKHR(surface);

    stable_sort(surfaceFormats.begin(), surfaceFormats.end(), [&](vk::SurfaceFormatKHR format1, vk::SurfaceFormatKHR format2) {
        const std::map<std::pair<vk::Format, vk::ColorSpaceKHR>, uint32_t> priority = {
            {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}, 10},
        };

        const auto it1 = priority.find({format1.format, format1.colorSpace});
        const auto priority_number1 = it1 == priority.end() ? 0 : it1->second;
        const auto it2 = priority.find({format2.format, format2.colorSpace});
        const auto priority_number2 = it2 == priority.end() ? 0 : it2->second;

        return priority_number1 > priority_number2;
    });
    stable_sort(surfacePresentModes.begin(), surfacePresentModes.end(), [&](vk::PresentModeKHR mode1, vk::PresentModeKHR mode2) {
        const std::map<vk::PresentModeKHR, uint32_t> priority = {
            {vk::PresentModeKHR::eMailbox, 10},
        };

        const auto it1 = priority.find(mode1);
        const auto priority_number1 = it1 == priority.end() ? 0 : it1->second;
        const auto it2 = priority.find(mode2);
        const auto priority_number2 = it2 == priority.end() ? 0 : it2->second;

        return priority_number1 > priority_number2;
    });

    vk::SurfaceFormatKHR swapchainFormat = surfaceFormats[0];
    vk::PresentModeKHR swapchainPresentMode = surfacePresentModes[0];

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
    swapchainCreateInfo.imageFormat = swapchainFormat.format;
    swapchainCreateInfo.imageColorSpace = swapchainFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.presentMode = swapchainPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;

    return SwapchainWithInfo{
        .format = swapchainFormat,
        .extent = surfaceCapabilities.currentExtent,
        .swapchain = device.createSwapchainKHRUnique(swapchainCreateInfo),
    };
}

vk::UniqueImageView create_image_view(vk::Device device, vk::Image image, vk::Format format) {
    vk::ImageViewCreateInfo create_info;
    create_info.image = image;
    create_info.viewType = vk::ImageViewType::e2D;
    create_info.format = format;
    create_info.components.r = vk::ComponentSwizzle::eIdentity;
    create_info.components.g = vk::ComponentSwizzle::eIdentity;
    create_info.components.b = vk::ComponentSwizzle::eIdentity;
    create_info.components.a = vk::ComponentSwizzle::eIdentity;
    create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    return device.createImageViewUnique(create_info);
}

std::vector<vk::UniqueImageView> create_image_views(vk::Device device, std::span<const vk::Image> images, vk::Format format) {
    std::vector<vk::UniqueImageView> image_views;
    std::transform(
        images.begin(), images.end(), std::back_inserter(image_views),
        [&](vk::Image image) {
            return create_image_view(device, image, format);
        });

    return image_views;
}

vk::UniqueFramebuffer create_frame_buf(vk::Device device, vk::ImageView image_view, const vk::Extent2D &extent, vk::RenderPass renderpass) {
    auto frameBufAttachments = {image_view};

    vk::FramebufferCreateInfo create_info;
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = 1;
    create_info.renderPass = renderpass;
    create_info.attachmentCount = uint32_t(frameBufAttachments.size());
    create_info.pAttachments = frameBufAttachments.begin();

    return device.createFramebufferUnique(create_info);
}

std::vector<vk::UniqueFramebuffer> create_frame_bufs(vk::Device device, std::span<const vk::UniqueImageView> image_views, const vk::Extent2D &extent, vk::RenderPass renderpass) {
    std::vector<vk::UniqueFramebuffer> frame_bufs;
    std::transform(
        image_views.begin(), image_views.end(), std::back_inserter(frame_bufs),
        [&](const vk::UniqueImageView &image_view) {
            return create_frame_buf(device, image_view.get(), extent, renderpass);
        });

    return frame_bufs;
}

vk::UniqueFence create_fence(vk::Device device, bool signaled) {
    vk::FenceCreateInfo create_info;
    if (signaled)
        create_info.flags = vk::FenceCreateFlagBits::eSignaled;

    return device.createFenceUnique(create_info);
}

std::vector<vk::UniqueFence> create_fences(vk::Device device, bool signaled, uint32_t num) {
    std::vector<vk::UniqueFence> fences;

    for (uint32_t i = 0; i < num; i++)
        fences.push_back(create_fence(device, signaled));
    return fences;
}

vk::UniqueSemaphore create_semaphore(vk::Device device) {
    vk::SemaphoreCreateInfo create_info;
    return device.createSemaphoreUnique(create_info);
}

std::vector<vk::UniqueSemaphore> create_semaphores(vk::Device device, uint32_t num) {
    std::vector<vk::UniqueSemaphore> semaphores;

    vk::SemaphoreCreateInfo create_info;
    for (uint32_t i = 0; i < num; i++)
        semaphores.push_back(device.createSemaphoreUnique(create_info));
    return semaphores;
}

std::pair<vma::UniqueBuffer, vma::UniqueAllocation> create_empty_buffer(vma::Allocator allocator, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo create_info;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.usage = vma::MemoryUsage::eAuto;

    return allocator.createBufferUnique(create_info, allocation_info);
}

std::pair<vma::UniqueBuffer, vma::UniqueAllocation> create_filled_buffer(vma::Allocator allocator, const uint8_t *p_data, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo create_info;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.flags |= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
    allocation_info.usage = vma::MemoryUsage::eAuto;

    auto buf = allocator.createBufferUnique(create_info, allocation_info);

    auto p_memory = allocator.mapMemory(buf.second.get());
    std::memcpy(p_memory, p_data, size);
    allocator.unmapMemory(buf.second.get());

    return buf;
}

BRIGHTCPP_GRAPHICS_VULKAN_END
