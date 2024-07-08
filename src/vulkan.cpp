#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <brightcpp/internal/vulkan.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vulkan/vulkan.hpp>

#include <battery/embed.hpp>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

auto instance_layer_required() {
    std::vector<const char *> layers;

#ifdef _DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    return layers;
}

auto instance_extension_required() {
    std::vector<const char *> exts;

    uint32_t glfw_required_exts_count;
    const auto glfw_required_exts = glfwGetRequiredInstanceExtensions(&glfw_required_exts_count);
    for (const auto ext : std::span{glfw_required_exts, glfw_required_exts_count})
        exts.push_back(ext);

    return exts;
}

auto device_layer_required() {
    std::vector<const char *> layers;

#ifdef _DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    return layers;
}

auto device_extension_required() {
    std::vector<const char *> exts = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    return exts;
}

static auto create_vulkan_instance() {
    auto layers = instance_layer_required();
    auto exts = instance_extension_required();

    vk::ApplicationInfo app_info;
    app_info.pApplicationName = "BrightCpp App";
    app_info.applicationVersion = 0;
    app_info.pEngineName = "BrightCpp";
    app_info.engineVersion = 0;

    vk::InstanceCreateInfo create_info;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = uint32_t(layers.size());
    create_info.ppEnabledLayerNames = layers.data();
    create_info.enabledExtensionCount = uint32_t(exts.size());
    create_info.ppEnabledExtensionNames = exts.data();

    return vk::createInstanceUnique(create_info);
}

struct queue_index_set {
    uint32_t graphics_queue, presentation_queue;
};

static auto choose_queue(vk::PhysicalDevice device, std::vector<vk::SurfaceKHR> surfaces_needed_support) {
    const auto queue_props = device.getQueueFamilyProperties();

    return std::optional(queue_index_set{
        .graphics_queue = 0,
        .presentation_queue = 0,
    });
}

static auto choose_phys_device(vk::Instance instance, const std::vector<vk::SurfaceKHR> &surfaces_needed_support) {
    const auto devices = instance.enumeratePhysicalDevices();
    const auto needed_exts = device_extension_required();

    for (const auto device : devices) {
        bool ok = true;

        const auto queue_indices = choose_queue(device, surfaces_needed_support);
        if (!queue_indices.has_value()) {
            ok = false;
        }

        const auto supported_exts = device.enumerateDeviceExtensionProperties();
        for (const auto &needed_ext : needed_exts) {
            auto is_supported =
                [needed_ext](vk::ExtensionProperties ext_prop) {
                    return std::strcmp(needed_ext, ext_prop.extensionName.data()) == 0;
                };

            if (std::find_if(supported_exts.begin(), supported_exts.end(), is_supported) == supported_exts.end()) {
                ok = false;
                break;
            }
        }

        if (ok) {
            return device;
        }
    }

    throw std::runtime_error("vulkan manager: suitable device not found");
}

static auto create_device(vk::PhysicalDevice phys_device, queue_index_set queue_indices) {
    std::vector<const char *> layers = device_layer_required();
    std::vector<const char *> exts = device_extension_required();

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
    float queuePriorities[1] = {1.0f};

    queueCreateInfo.push_back(
        vk::DeviceQueueCreateInfo{}
            .setQueueFamilyIndex(queue_indices.graphics_queue)
            .setQueueCount(1)
            .setPQueuePriorities(queuePriorities));

    if (queue_indices.presentation_queue != queue_indices.graphics_queue) {
        queueCreateInfo.push_back(
            vk::DeviceQueueCreateInfo{}
                .setQueueFamilyIndex(queue_indices.presentation_queue)
                .setQueueCount(1)
                .setPQueuePriorities(queuePriorities));
    }

    vk::DeviceCreateInfo create_info;
    create_info.queueCreateInfoCount = uint32_t(queueCreateInfo.size());
    create_info.pQueueCreateInfos = queueCreateInfo.data();
    create_info.enabledLayerCount = uint32_t(layers.size());
    create_info.ppEnabledLayerNames = layers.data();
    create_info.enabledExtensionCount = uint32_t(exts.size());
    create_info.ppEnabledExtensionNames = exts.data();

    return phys_device.createDeviceUnique(create_info);
}

auto create_tmp_cmd_pool(vk::Device device, const queue_index_set &queue_indices) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

auto create_draw_cmd_pool(vk::Device device, const queue_index_set &queue_indices) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

auto create_cmd_buf(vk::Device device, vk::CommandPool pool) {
    vk::CommandBufferAllocateInfo alloc_info;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;

    return device.allocateCommandBuffersUnique(alloc_info);
}

auto create_render_pass(vk::Device device) {
    vk::AttachmentDescription attachments[1];
    attachments[0].format = vk::Format::eR8G8B8A8Unorm;
    attachments[0].samples = vk::SampleCountFlagBits::e1;
    attachments[0].loadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[0].initialLayout = vk::ImageLayout::eUndefined;
    attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference subpass0_attachmentRefs[1];
    subpass0_attachmentRefs[0].attachment = 0;
    subpass0_attachmentRefs[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpasses[1];
    subpasses[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = subpass0_attachmentRefs;

    vk::RenderPassCreateInfo renderpassCreateInfo;
    renderpassCreateInfo.attachmentCount = 1;
    renderpassCreateInfo.pAttachments = attachments;
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = subpasses;
    renderpassCreateInfo.dependencyCount = 0;
    renderpassCreateInfo.pDependencies = nullptr;

    return device.createRenderPassUnique(renderpassCreateInfo);
}

auto create_vert_shader(vk::Device device) {
    const auto data = b::embed<"shaders/shader.vert.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

auto create_frag_shader(vk::Device device) {
    const auto data = b::embed<"shaders/shader.frag.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

auto create_pipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::ShaderModule vert_shader, vk::ShaderModule frag_shader) {
    vk::Viewport viewports[1];
    viewports[0].x = 0.0;
    viewports[0].y = 0.0;
    viewports[0].minDepth = 0.0;
    viewports[0].maxDepth = 1.0;
    viewports[0].width = float(extent.width);
    viewports[0].height = float(extent.height);

    vk::Rect2D scissors[1];
    scissors[0].offset = vk::Offset2D{0, 0};
    scissors[0].extent = extent;

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = viewports;
    viewportState.scissorCount = 1;
    viewportState.pScissors = scissors;

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = false;
    rasterizer.rasterizerDiscardEnable = false;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = false;

    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample.sampleShadingEnable = false;
    multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState blendattachment[1];
    blendattachment[0].colorWriteMask =
        vk::ColorComponentFlagBits::eA |
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB;
    blendattachment[0].blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    blend.attachmentCount = 1;
    blend.pAttachments = blendattachment;

    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = nullptr;

    vk::UniquePipelineLayout pipelineLayout = device.createPipelineLayoutUnique(layoutCreateInfo);

    auto shader_stages = {
        vk::PipelineShaderStageCreateInfo{}
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vert_shader)
            .setPName("main"),
        vk::PipelineShaderStageCreateInfo{}
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(frag_shader)
            .setPName("main"),
    };

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pRasterizationState = &rasterizer;
    pipelineCreateInfo.pMultisampleState = &multisample;
    pipelineCreateInfo.pColorBlendState = &blend;
    pipelineCreateInfo.layout = pipelineLayout.get();
    pipelineCreateInfo.stageCount = uint32_t(shader_stages.size());
    pipelineCreateInfo.pStages = shader_stages.begin();
    pipelineCreateInfo.renderPass = renderpass;
    pipelineCreateInfo.subpass = 0;

    return device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
}

struct SwapchainWithInfo {
    vk::SurfaceFormatKHR format;
    vk::Extent2D extent;
    vk::UniqueSwapchainKHR swapchain;
};

auto create_swapchain(vk::Device device, vk::PhysicalDevice phys_device, vk::SurfaceKHR surface) {
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

auto create_image_view(vk::Device device, vk::Image image, vk::Format format) {
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

auto create_image_views(vk::Device device, std::span<vk::Image> images, vk::Format format) {
    std::vector<vk::UniqueImageView> image_views;
    std::transform(
        images.begin(), images.end(), std::back_inserter(image_views),
        [&](vk::Image image) {
            return create_image_view(device, image, format);
        });

    return image_views;
}

auto create_frame_buf(vk::Device device, vk::ImageView image_view, const vk::Extent2D &extent, vk::RenderPass renderpass) {
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

auto create_frame_bufs(vk::Device device, std::span<const vk::UniqueImageView> image_views, const vk::Extent2D &extent, vk::RenderPass renderpass) {
    std::vector<vk::UniqueFramebuffer> frame_bufs;
    std::transform(
        image_views.begin(), image_views.end(), std::back_inserter(frame_bufs),
        [&](const vk::UniqueImageView &image_view) {
            return create_frame_buf(device, image_view.get(), extent, renderpass);
        });

    return frame_bufs;
}

class render_target {
    vk::UniqueSurfaceKHR surface;
    SwapchainWithInfo swapchain;
    std::vector<vk::Image> swapchain_images;
    std::vector<vk::UniqueImageView> swapchain_imageviews;

  public:
    // this handles ownership of surface
    render_target(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device, vk::SurfaceKHR surface)
        : surface{surface, instance},
          swapchain{create_swapchain(device, phys_device, surface)},
          swapchain_images{device.getSwapchainImagesKHR(swapchain.swapchain.get())},
          swapchain_imageviews{create_image_views(device, swapchain_images, swapchain.format.format)} {}

    const auto &image_views() const { return swapchain_imageviews; }
    auto extent() const { return swapchain.extent; }
};

class render_proc {
    vk::UniqueRenderPass renderpass;
    vk::UniqueShaderModule vert_shader;
    vk::UniqueShaderModule frag_shader;
    vk::UniquePipeline pipeline;
    std::vector<vk::UniqueFramebuffer> framebufs;

  public:
    render_proc(vk::Device device, const render_target &rt)
        : renderpass{create_render_pass(device)},
          vert_shader{create_vert_shader(device)},
          frag_shader{create_frag_shader(device)},
          pipeline{create_pipeline(device, renderpass.get(), rt.extent(), vert_shader.get(), frag_shader.get())},
          framebufs{create_frame_bufs(device, rt.image_views(), rt.extent(), renderpass.get())} {}
};

class vulkan_manager : public system_module {
    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vk::UniqueCommandPool tmp_cmd_pool;
    vk::UniqueCommandPool draw_cmd_pool;
    std::vector<vk::UniqueCommandBuffer> tmp_cmd_buf;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buf;
    std::vector<vk::SurfaceKHR> surface_needed_support;

  public:
    vulkan_manager()
        : instance{create_vulkan_instance()},
          phys_device{choose_phys_device(instance.get(), {})},
          queue_indices{choose_queue(phys_device, {}).value()},
          device{create_device(phys_device, queue_indices)},
          graphics_queue{device->getQueue(queue_indices.graphics_queue, 0)},
          presentation_queue{device->getQueue(queue_indices.presentation_queue, 0)},
          tmp_cmd_pool{create_tmp_cmd_pool(device.get(), queue_indices)},
          tmp_cmd_buf{create_cmd_buf(device.get(), tmp_cmd_pool.get())},
          draw_cmd_pool{create_tmp_cmd_pool(device.get(), queue_indices)},
          draw_cmd_buf{create_cmd_buf(device.get(), draw_cmd_pool.get())} {}

    auto create_render_target_from_glfw_window(GLFWwindow *window) {
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(instance.get(), window, nullptr, &surface);

        return render_target(instance.get(), phys_device, device.get(), surface);
    }

    auto create_render_proc(const render_target &rt) {
        return render_proc(device.get(), rt);
    }
};

std::optional<vulkan_manager> g_vulkan_manager;

void setup_vulkan_manager() {
    g_vulkan_manager.emplace();
#ifdef _DEBUG
    std::cout << "vulkan initialized." << std::endl;
#endif
}
void shutdown_vulkan_manager() {
#ifdef _DEBUG
    std::cout << "vulkan shutdown..." << std::endl;
#endif
    g_vulkan_manager.reset();
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
