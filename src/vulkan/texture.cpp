#include <brightcpp/internal/vulkan/vma.hpp>

auto create_allocator(vk::Instance instance, vk::Device device) {
    vma::AllocatorCreateInfo create_info;
    create_info.device = device;
    create_info.instance = instance;

    return vma::createAllocatorUnique(create_info);
}

auto cmd_change_image_layout(vk::CommandBuffer cmd_buf, vk::Image image, vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
    vk::ImageMemoryBarrier barrior;
    barrior.oldLayout = old_layout;
    barrior.newLayout = new_layout;
    barrior.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrior.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrior.image = image;
    barrior.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrior.subresourceRange.baseMipLevel = 0;
    barrior.subresourceRange.levelCount = 1;
    barrior.subresourceRange.baseArrayLayer = 0;
    barrior.subresourceRange.layerCount = 1;
    barrior.srcAccessMask = {};
    barrior.dstAccessMask = {};

    vk::PipelineStageFlags src_stage = {}, dst_stage = {};

    if (new_layout == vk::ImageLayout::eTransferDstOptimal) {
        barrior.dstAccessMask |= vk::AccessFlagBits::eTransferWrite;
        dst_stage |= vk::PipelineStageFlagBits::eTransfer;
    }
    if (old_layout == vk::ImageLayout::eTransferDstOptimal) {
        barrior.srcAccessMask |= vk::AccessFlagBits::eTransferWrite;
        src_stage |= vk::PipelineStageFlagBits::eTransfer;
    }
    if (new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrior.dstAccessMask |= vk::AccessFlagBits::eShaderRead;
        dst_stage |= vk::PipelineStageFlagBits::eFragmentShader;
    }

    cmd_buf.pipelineBarrier(src_stage, dst_stage, {}, {}, {}, {barrior});
}

auto cmd_copy_buffer_to_image(vk::CommandBuffer cmd_buf, vk::Buffer src_buf, vk::Image dst_image, uint32_t w, uint32_t h) {
    vk::BufferImageCopy imgCopyRegion;
    imgCopyRegion.bufferOffset = 0;
    imgCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    imgCopyRegion.imageSubresource.mipLevel = 0;
    imgCopyRegion.imageSubresource.baseArrayLayer = 0;
    imgCopyRegion.imageSubresource.layerCount = 1;
    imgCopyRegion.imageOffset = vk::Offset3D{0, 0, 0};
    imgCopyRegion.imageExtent = vk::Extent3D{w, h, 1};

    imgCopyRegion.bufferRowLength = 0;
    imgCopyRegion.bufferImageHeight = 0;
    cmd_buf.copyBufferToImage(src_buf, dst_image, vk::ImageLayout::eTransferDstOptimal, imgCopyRegion);
}

auto create_empty_image(vma::Allocator allocator, const uint8_t *data, int w, int h, vk::ImageUsageFlags usage) {
    vk::ImageCreateInfo create_info;
    create_info.imageType = vk::ImageType::e2D;
    create_info.extent = vk::Extent3D(w, h, 1);
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.format = vk::Format::eR8G8B8A8Unorm;
    create_info.tiling = vk::ImageTiling::eOptimal;
    create_info.initialLayout = vk::ImageLayout::eUndefined;
    create_info.usage = usage;
    create_info.sharingMode = vk::SharingMode::eExclusive;
    create_info.samples = vk::SampleCountFlagBits::e1;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.usage = vma::MemoryUsage::eAuto;
    allocation_info.flags = vma::AllocationCreateFlagBits::eDedicatedMemory;

    return allocator.createImageUnique(create_info, allocation_info);
}

auto create_empty_buffer(vma::Allocator allocator, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo create_info;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.usage = vma::MemoryUsage::eAuto;

    return allocator.createBufferUnique(create_info, allocation_info);
}

auto create_filled_buffer(vma::Allocator allocator, const uint8_t *p_data, vk::DeviceSize size, vk::BufferUsageFlags usage) {
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

auto create_texture(vma::Allocator allocator, const uint8_t *data, uint32_t w, uint32_t h, vk::Queue queue, vk::CommandBuffer cmd_buf) {
    auto [tmp_buf, buf_allocation] =
        create_filled_buffer(
            allocator, data, w * h * 4,
            vk::BufferUsageFlagBits::eTransferSrc);

    auto [image, image_allocation] =
        create_empty_image(
            allocator, data, w, h,
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);

    {
        vk::CommandBufferBeginInfo cmd_begin_info;
        cmd_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        cmd_buf.begin(cmd_begin_info);

        cmd_change_image_layout(cmd_buf, image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        cmd_copy_buffer_to_image(cmd_buf, tmp_buf.get(), image.get(), w, h);
        cmd_change_image_layout(cmd_buf, image.get(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        cmd_buf.end();

        vk::SubmitInfo submit_info;
        auto submit_cmd_buf = {cmd_buf};
        submit_info.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submit_info.pCommandBuffers = submit_cmd_buf.begin();

        queue.submit({submit_info});
    }

    return std::make_pair(std::move(image), std::move(image_allocation));
}
