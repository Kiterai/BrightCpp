#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <brightcpp/internal/vulkan.hpp>
#include <brightcpp/internal/vulkan/common.hpp>
#include <brightcpp/internal/vulkan/render_proc.hpp>
#include <brightcpp/internal/vulkan/render_target.hpp>
#include <brightcpp/internal/vulkan/texture.hpp>
#include <brightcpp/internal/vulkan/vma.hpp>
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

static auto create_allocator(vk::Instance instance, vk::PhysicalDevice phys_device, vk::Device device) {
    vma::AllocatorCreateInfo create_info;
    create_info.device = device;
    create_info.physicalDevice = phys_device;
    create_info.instance = instance;

    return vma::createAllocatorUnique(create_info);
}

class vulkan_manager {
    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vma::UniqueAllocator allocator;
    texture_factory tex_factory;
    std::vector<vk::SurfaceKHR> surface_needed_support;

  public:
    vulkan_manager()
        : instance{create_vulkan_instance()},
          phys_device{choose_phys_device(instance.get(), {})},
          queue_indices{choose_queue(phys_device, {}).value()},
          device{create_device(phys_device, queue_indices)},
          graphics_queue{device->getQueue(queue_indices.graphics_queue, 0)},
          presentation_queue{device->getQueue(queue_indices.presentation_queue, 0)},
          allocator{create_allocator(instance.get(), phys_device, device.get())},
          tex_factory{device.get(), allocator.get(), queue_indices} {}
    ~vulkan_manager() {
        wait_idle();
    }

    auto create_render_target_from_glfw_window(GLFWwindow *window) {
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(instance.get(), window, nullptr, &surface);

        return render_target(instance.get(), phys_device, device.get(), surface);
    }

    auto create_render_proc(const render_target &rt) {
        return render_proc(device.get(), rt, queue_indices);
    }

    void wait_idle() {
        presentation_queue.waitIdle();
        graphics_queue.waitIdle();
    }
};

std::optional<vulkan_manager> g_vulkan_manager;
std::map<GLFWwindow *, render_target> render_targets;
std::map<render_target *, render_proc> render_procs;
render_target *current_render_target = nullptr;

void create_render_target(GLFWwindow *window) {
    auto [elem, result] = render_targets.try_emplace(window, g_vulkan_manager->create_render_target_from_glfw_window(window));

    render_procs.emplace(
        &elem->second,
        g_vulkan_manager->create_render_proc(elem->second));
}
void destroy_render_target(GLFWwindow *window) {
    const auto it = render_procs.find(current_render_target);
    it->second.render_end(*current_render_target);

    render_procs.erase(it->first);
    render_targets.erase(window);
}
void set_current_render_target(GLFWwindow *window) {
    if (current_render_target) {
        const auto it = render_procs.find(current_render_target);
        it->second.render_end(*current_render_target);
    }

    current_render_target = &render_targets[window];

    if (current_render_target) {
        const auto it = render_procs.find(current_render_target);
        it->second.render_begin(*current_render_target);
    }
}
void apply_render() {
    if (current_render_target) {
        const auto it = render_procs.find(current_render_target);
        it->second.render_end(*current_render_target);
        it->second.render_begin(*current_render_target);
    }
}

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
    g_vulkan_manager->wait_idle();
    render_procs.clear();
    render_targets.clear();
    g_vulkan_manager.reset();
}

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
