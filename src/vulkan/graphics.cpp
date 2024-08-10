#include "graphics.hpp"
#include "render_proc_2d.hpp"
#include "render_target.hpp"
#include "texture.hpp"
#include "util.hpp"
#include "vma.hpp"
#include <iostream>
#include <list>
#include <optional>
#include <vulkan/vulkan.hpp>

#include <battery/embed.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

auto instance_layer_required() {
    std::vector<const char *> layers;

#ifdef _DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    return layers;
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

static auto create_vulkan_instance(os_util_backend &os_util) {
    auto layers = instance_layer_required();
    auto exts = os_util.get_vulkan_required_instance_extensions();

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

class vulkan_manager : public graphics_backend {
    std::shared_ptr<os_util_backend> os_util;

    vk::UniqueInstance instance;
    vk::PhysicalDevice phys_device;
    queue_index_set queue_indices;
    vk::UniqueDevice device;
    vk::Queue graphics_queue, presentation_queue;
    vma::UniqueAllocator allocator;

    std::vector<vk::SurfaceKHR> surface_needed_support;

    using handle_t = handle_holder<render_target>;
    using handle_value_t = handle_t::handle_value_t;
    std::unordered_map<handle_value_t, vulkan::render_target_vulkan> rendertarget_db;

  public:
    vulkan_manager(const std::shared_ptr<os_util_backend> &_os_util)
        : os_util{_os_util},
          instance{create_vulkan_instance(*os_util)},
          phys_device{choose_phys_device(instance.get(), {})},
          queue_indices{choose_queue(phys_device, {}).value()},
          device{create_device(phys_device, queue_indices)},
          graphics_queue{device->getQueue(queue_indices.graphics_queue, 0)},
          presentation_queue{device->getQueue(queue_indices.presentation_queue, 0)},
          allocator{create_allocator(instance.get(), phys_device, device.get())} {}
    ~vulkan_manager() {
        wait_idle();
    }

    auto create_render_proc(const render_target_vulkan &rt) {
        return render_proc_2d(device.get(), rt, queue_indices);
    }

    void wait_idle() {
        presentation_queue.waitIdle();
        graphics_queue.waitIdle();
    }

    handle_holder<render_target_vulkan>::handle_value_t create_render_target(window_backend &window) {
        const auto handle = rendertarget_db.size();

        rendertarget_db.insert({
            handle,
            render_target_vulkan(
                instance.get(),
                phys_device,
                device.get(),
                window.get_vulkan_surface(instance.get())),
        });
        return handle;
    }
    void destroy_render_target(handle_holder<render_target> &rt) noexcept override {
        rendertarget_db.erase(rt.handle());
    }

    std::unique_ptr<texture_factory_backend> create_texture_factory() override {
        return std::make_unique<texture_factory>(device.get(), allocator.get(), queue_indices);
    }

    void draw(texture_backend texture, render_rect_info &info) override {
    };
};

std::unique_ptr<graphics_backend> make_graphics_vulkan(const std::shared_ptr<os_util_backend> &os_util) {
    return std::make_unique<vulkan_manager>(os_util);
}

BRIGHTCPP_GRAPHICS_VULKAN_END
