#include <brightcpp/internal/vulkan/common.hpp>
#include "texture.hpp"

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

static auto create_sampler(vk::Device device) {
    vk::SamplerCreateInfo create_info;
    create_info.magFilter = vk::Filter::eNearest;
    create_info.minFilter = vk::Filter::eNearest;
    create_info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    create_info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    create_info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    create_info.anisotropyEnable = false;
    create_info.maxAnisotropy = 1.0f;
    create_info.borderColor = vk::BorderColor::eFloatTransparentBlack;
    create_info.unnormalizedCoordinates = false;
    create_info.compareEnable = false;
    create_info.compareOp = vk::CompareOp::eAlways;
    create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    create_info.mipLodBias = 0.0f;
    create_info.minLod = 0.0f;
    create_info.maxLod = 0.0f;

    return device.createSamplerUnique(create_info);
}

static auto cmd_change_image_layout(vk::CommandBuffer cmd_buf, vk::Image image, vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
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

    switch (old_layout) {
    case vk::ImageLayout::eTransferDstOptimal:
        barrior.srcAccessMask |= vk::AccessFlagBits::eTransferWrite;
        src_stage |= vk::PipelineStageFlagBits::eTransfer;
        break;
    default:
        src_stage |= vk::PipelineStageFlagBits::eTopOfPipe;
        break;
    }
    switch (new_layout) {
    case vk::ImageLayout::eTransferDstOptimal:
        barrior.dstAccessMask |= vk::AccessFlagBits::eTransferWrite;
        dst_stage |= vk::PipelineStageFlagBits::eTransfer;
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        barrior.dstAccessMask |= vk::AccessFlagBits::eShaderRead;
        dst_stage |= vk::PipelineStageFlagBits::eFragmentShader;
        break;
    default:
        break;
    }

    cmd_buf.pipelineBarrier(src_stage, dst_stage, {}, {}, {}, {barrior});
}

static auto cmd_copy_buffer_to_image(vk::CommandBuffer cmd_buf, vk::Buffer src_buf, vk::Image dst_image, uint32_t w, uint32_t h) {
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

static auto create_empty_image(vma::Allocator allocator, const uint8_t *data, int w, int h, vk::ImageUsageFlags usage) {
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

static auto create_empty_buffer(vma::Allocator allocator, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    vk::BufferCreateInfo create_info;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo allocation_info;
    allocation_info.usage = vma::MemoryUsage::eAuto;

    return allocator.createBufferUnique(create_info, allocation_info);
}

static auto create_filled_buffer(vma::Allocator allocator, const uint8_t *p_data, vk::DeviceSize size, vk::BufferUsageFlags usage) {
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

static auto create_descriptorset_layout(vk::Device device) {
    auto bindings = {
        vk::DescriptorSetLayoutBinding{}
            .setBinding(0)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment),
    };

    vk::DescriptorSetLayoutCreateInfo create_info;
    create_info.bindingCount = uint32_t(bindings.size());
    create_info.pBindings = bindings.begin();

    return device.createDescriptorSetLayoutUnique(create_info);
}

static auto create_descriptor_pool(vk::Device device) {
    auto pool_sizes = {
        vk::DescriptorPoolSize{}
            .setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(256),
    };

    vk::DescriptorPoolCreateInfo create_info;
    create_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    create_info.poolSizeCount = uint32_t(pool_sizes.size());
    create_info.pPoolSizes = pool_sizes.begin();
    create_info.maxSets = 256;

    return device.createDescriptorPoolUnique(create_info);
}

texture_factory::texture_factory(vk::Device device, vma::Allocator allocator, const queue_index_set &queue_indices)
    : device{device},
      allocator{allocator},
      sampler{create_sampler(device)},
      desc_layout{create_descriptorset_layout(device)},
      desc_pool{create_descriptor_pool(device)},
      queue{device.getQueue(queue_indices.graphics_queue, 0)},
      cmd_pool{create_cmd_pool(device, queue_indices, vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer)},
      cmd_buf{std::move(create_cmd_bufs(device, cmd_pool.get(), 1)[0])},
      texture_creating_fence{create_fence(device, false)} {}

vk::UniqueDescriptorSet texture_factory::create_texture_descriptor_set(vk::ImageView image_view) {
    auto layouts = {desc_layout.get()};

    vk::DescriptorSetAllocateInfo allocate_info;
    allocate_info.descriptorPool = desc_pool.get();
    allocate_info.descriptorSetCount = uint32_t(layouts.size());
    allocate_info.pSetLayouts = layouts.begin();

    auto desc_set = std::move(device.allocateDescriptorSetsUnique(allocate_info)[0]);

    auto images_info = {
        vk::DescriptorImageInfo{}
            .setImageView(image_view)
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSampler(sampler.get()),
    };

    vk::WriteDescriptorSet write;
    write.dstSet = desc_set.get();
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = uint32_t(images_info.size());
    write.pImageInfo = images_info.begin();

    device.updateDescriptorSets({write}, {});

    return desc_set;
}

texture_resource texture_factory::create_texture(const uint8_t *data, uint32_t w, uint32_t h) {
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

        cmd_buf->begin(cmd_begin_info);

        cmd_change_image_layout(cmd_buf.get(), image.get(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        cmd_copy_buffer_to_image(cmd_buf.get(), tmp_buf.get(), image.get(), w, h);
        cmd_change_image_layout(cmd_buf.get(), image.get(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        cmd_buf->end();

        vk::SubmitInfo submit_info;
        auto submit_cmd_buf = {cmd_buf.get()};
        submit_info.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submit_info.pCommandBuffers = submit_cmd_buf.begin();

        queue.submit({submit_info}, texture_creating_fence.get());
    }

    auto image_view = create_image_view(device, image.get(), vk::Format::eR8G8B8A8Unorm);

    auto desc_set = create_texture_descriptor_set(image_view.get());

    device.waitForFences({texture_creating_fence.get()}, VK_TRUE, UINT64_MAX);
    device.resetFences({texture_creating_fence.get()});

    return {
        .image = std::move(image),
        .allocation = std::move(image_allocation),
        .image_view = std::move(image_view),
        .desc_set = std::move(desc_set),
    };
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
