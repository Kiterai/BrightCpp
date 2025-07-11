#include "renderer2d.hpp"
#include "../../global_module.hpp"
#include "../../linear_algebra.hpp"
#include "graphics.hpp"
#include "texture.hpp"
#include "util.hpp"
#include <cmath>
#include <iostream>

#include <battery/embed.hpp>

BRIGHTCPP_GRAPHICS_VULKAN_START

constexpr uint32_t frames_inflight = 2;
constexpr handle_holder<image_impl>::handle_value_t no_bind_texture = std::numeric_limits<size_t>::max();

struct shader_pushconstant {
    mat3 draw_matrix;
    vec2 screen_size;
    vec2 tex_clip_pos;
    vec2 tex_clip_size;
    vec4 color;
};

static auto create_draw_cmd_pool(vk::Device device, const queue_index_set &queue_indices) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

static auto create_render_pass(vk::Device device, vk::Format format) {
    vk::AttachmentDescription attachments[1];
    attachments[0].format = format;
    attachments[0].samples = vk::SampleCountFlagBits::e1;
    attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
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

static auto create_vert_shader(vk::Device device) {
    const auto data = b::embed<"graphics/shaders/shader.vert.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

static auto create_frag_shader(vk::Device device) {
    const auto data = b::embed<"graphics/shaders/shader.frag.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

static auto create_pipeline_layout(vk::Device device) {
    auto pushConstantRanges = {
        vk::PushConstantRange{}
            .setOffset(0)
            .setSize(sizeof(shader_pushconstant))
            .setStageFlags(vk::ShaderStageFlagBits::eVertex),
    };

    auto setLayouts = {
        global_module<texture_factory_vulkan>::get().get_descriptor_set_layout(),
    };

    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.setLayoutCount = uint32_t(setLayouts.size());
    layoutCreateInfo.pSetLayouts = setLayouts.begin();
    layoutCreateInfo.pPushConstantRanges = pushConstantRanges.begin();
    layoutCreateInfo.pushConstantRangeCount = uint32_t(pushConstantRanges.size());

    return device.createPipelineLayoutUnique(layoutCreateInfo);
}

static auto create_pipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::PipelineLayout pipeline_layout, vk::ShaderModule vert_shader, vk::ShaderModule frag_shader, bool alpha = true) {
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
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleStrip;
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
    blendattachment[0].blendEnable = alpha;
    if (alpha) {
        blendattachment[0].srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        blendattachment[0].dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        blendattachment[0].colorBlendOp = vk::BlendOp::eAdd;
        blendattachment[0].srcAlphaBlendFactor = vk::BlendFactor::eOne;
        blendattachment[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;
        blendattachment[0].alphaBlendOp = vk::BlendOp::eAdd;
    }

    vk::PipelineColorBlendStateCreateInfo blend;
    blend.logicOpEnable = false;
    blend.attachmentCount = 1;
    blend.pAttachments = blendattachment;

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
    pipelineCreateInfo.layout = pipeline_layout;
    pipelineCreateInfo.stageCount = uint32_t(shader_stages.size());
    pipelineCreateInfo.pStages = shader_stages.begin();
    pipelineCreateInfo.renderPass = renderpass;
    pipelineCreateInfo.subpass = 0;

    return device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
}

renderer2d_vulkan::renderer2d_vulkan(vk::Device device, render_target_vulkan &_rt, const queue_index_set &queue_indices)
    : rt{_rt},
      device{device},
      renderpass{create_render_pass(device, rt.get().format())},
      vert_shader{create_vert_shader(device)},
      frag_shader{create_frag_shader(device)},
      pipeline_layout{create_pipeline_layout(device)},
      pipeline{create_pipeline(device, renderpass.get(), rt.get().extent(), pipeline_layout.get(), vert_shader.get(), frag_shader.get())},
      framebufs{create_frame_bufs(device, rt.get().image_views(), rt.get().extent(), renderpass.get())} {}

renderer2d_vulkan::~renderer2d_vulkan() {
    if (rendering) {
        render_end();
        rt.get().render_end();
    }
    rt.get().wait_idle();
}

void renderer2d_vulkan::render_begin() {
    assert(!rendering);
    rendering = true;

    const auto begin_info = rt.get().render_begin();
    cmd_buf = begin_info.cmd_buf;

    vk::RenderPassBeginInfo renderpassBeginInfo;
    renderpassBeginInfo.renderPass = renderpass.get();
    renderpassBeginInfo.framebuffer = framebufs[begin_info.img_index].get();
    renderpassBeginInfo.renderArea = vk::Rect2D({0, 0}, rt.get().extent());

    auto clearVal = {
        vk::ClearValue{}
            .setColor(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f)),
    };

    renderpassBeginInfo.clearValueCount = uint32_t(clearVal.size());
    renderpassBeginInfo.pClearValues = clearVal.begin();

    cmd_buf.beginRenderPass(renderpassBeginInfo, vk::SubpassContents::eInline);

    cmd_buf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());

    last_binded_texture = no_bind_texture;
}
void renderer2d_vulkan::render_end() {
    assert(rendering);
    rendering = false;

    cmd_buf.endRenderPass();
    rt.get().render_end();
}

void renderer2d_vulkan::draw_texture(handle_holder<image_impl> image, const render_texture_info &rect_info) {
    const auto &texture = global_module<texture_factory_vulkan>::get().get(image);

    if (last_binded_texture != image.handle()) {
        cmd_buf.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline_layout.get(),
            0,
            {texture.desc_set.get()},
            {});
        last_binded_texture = image.handle();
    }

    // gcc not supports cosf, sinf
    const auto
        cos_th = std::cos(rect_info.theta),
        sin_th = std::sin(rect_info.theta);

    const auto anchor_x = rect_info.anchor_pos.v[0];
    const auto anchor_y = rect_info.anchor_pos.v[1];
    const auto w = rect_info.clip_size.v[0] * rect_info.scale.v[0];
    const auto h = rect_info.clip_size.v[1] * rect_info.scale.v[1];
    const auto x = rect_info.pos.v[0];
    const auto y = rect_info.pos.v[1];

    mat3 pivot_mat{.m{
        {1.0f, 0.0f, -anchor_x},
        {0.0f, 1.0f, -anchor_y},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 scale_mat{.m{
        {w, 0.0f, 0.0f},
        {0.0f, h, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 rotate_mat{.m{
        {cos_th, -sin_th, 0.0f},
        {sin_th, cos_th, 0.0f},
        {0.0f, 0.0f, 1.0f},
    }};
    mat3 move_mat{.m{
        {1.0f, 0.0f, x},
        {0.0f, 1.0f, y},
        {0.0f, 0.0f, 1.0f},
    }};

    const auto tex_w = texture.w;
    const auto tex_h = texture.h;
    const shader_pushconstant data{
        .draw_matrix{move_mat * rotate_mat * scale_mat * pivot_mat},
        .screen_size{
            .v{float(rt.get().extent().width), float(rt.get().extent().height)},
        },
        .tex_clip_pos{
            .v{rect_info.clip_pos.v[0] / tex_w, rect_info.clip_pos.v[1] / tex_h},
        },
        .tex_clip_size{
            .v{rect_info.clip_size.v[0] / tex_w, rect_info.clip_size.v[1] / tex_h},
        },
        .color{rect_info.color},
    };
    cmd_buf.pushConstants<shader_pushconstant>(pipeline_layout.get(), vk::ShaderStageFlagBits::eVertex, 0, {data});
    cmd_buf.draw(4, 1, 0, 0);
}

renderer2d_factory_vulkan::renderer2d_factory_vulkan()
    : device{global_module<graphics_vulkan>::get().get_device()},
      queue_indices{global_module<graphics_vulkan>::get().get_queue_indices()} {}

std::unique_ptr<renderer2d_backend> renderer2d_factory_vulkan::make(render_target rt) {
    return std::make_unique<renderer2d_vulkan>(
        device,
        global_module<graphics_vulkan>::get().get_render_target_vulkan(rt),
        queue_indices);
}

BRIGHTCPP_GRAPHICS_VULKAN_END
