#include <GLFW/glfw3.h>
#include <brightcpp/internal/vulkan.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <vulkan/vulkan.hpp>

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
    create_info.queueCreateInfoCount = queueCreateInfo.size();
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
    vk::UniqueRenderPass renderpass;
    vk::UniqueShaderModule vert_shader;
    vk::UniqueShaderModule frag_shader;
    vk::UniquePipeline pipeline;
    vk::UniqueFramebuffer framebuf;
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
          draw_cmd_buf{create_cmd_buf(device.get(), draw_cmd_pool.get())},
          renderpass{create_render_pass(device.get())} {
#ifdef _DEBUG
        std::cout << "vulkan initialized." << std::endl;
#endif
    }
};

std::unique_ptr<system_module> make_vulkan_manager() {
    return std::make_unique<vulkan_manager>();
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
