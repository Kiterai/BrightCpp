#include <brightcpp/internal/linear_algebra.hpp>
#include <brightcpp/internal/vulkan/common.hpp>
#include <brightcpp/internal/vulkan/render_proc.hpp>
#include <brightcpp/internal/vulkan/render_target.hpp>

#include <battery/embed.hpp>

struct shader_pushconstant {
    mat3 draw_matrix;
    vec2 screen_size;
    vec2 tex_clip_pos;
    vec2 tex_clip_size;
};

auto create_draw_cmd_pool(vk::Device device, const queue_index_set &queue_indices) {
    vk::CommandPoolCreateInfo create_info;
    create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    create_info.queueFamilyIndex = queue_indices.graphics_queue;

    return device.createCommandPoolUnique(create_info);
}

vk::UniqueRenderPass create_render_pass(vk::Device device, vk::Format format) {
    vk::AttachmentDescription attachments[1];
    attachments[0].format = format;
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

vk::UniqueShaderModule create_vert_shader(vk::Device device) {
    const auto data = b::embed<"shaders/shader.vert.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

vk::UniqueShaderModule create_frag_shader(vk::Device device) {
    const auto data = b::embed<"shaders/shader.frag.spv">();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = data.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(data.data());

    return device.createShaderModuleUnique(create_info);
}

vk::UniquePipelineLayout create_pipeline_layout(vk::Device device) {
    auto pushConstantRanges = {
        vk::PushConstantRange{}
            .setOffset(0)
            .setSize(sizeof(shader_pushconstant))
            .setStageFlags(vk::ShaderStageFlagBits::eVertex),
    };

    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = nullptr;
    layoutCreateInfo.pPushConstantRanges = pushConstantRanges.begin();
    layoutCreateInfo.pushConstantRangeCount = uint32_t(pushConstantRanges.size());

    return device.createPipelineLayoutUnique(layoutCreateInfo);
}

vk::UniquePipeline create_pipeline(vk::Device device, vk::RenderPass renderpass, vk::Extent2D extent, vk::PipelineLayout pipeline_layout, vk::ShaderModule vert_shader, vk::ShaderModule frag_shader) {
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
    blendattachment[0].blendEnable = false;

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

render_proc::render_proc(vk::Device device, const render_target &rt, const queue_index_set &queue_indices)
    : device{device},
      renderpass{create_render_pass(device, rt.format())},
      vert_shader{create_vert_shader(device)},
      frag_shader{create_frag_shader(device)},
      pipeline_layout{create_pipeline_layout(device)},
      pipeline{create_pipeline(device, renderpass.get(), rt.extent(), pipeline_layout.get(), vert_shader.get(), frag_shader.get())},
      framebufs{create_frame_bufs(device, rt.image_views(), rt.extent(), renderpass.get())},
      draw_cmd_pool{create_draw_cmd_pool(device, queue_indices)},
      draw_cmd_buf{create_cmd_bufs(device, draw_cmd_pool.get(), uint32_t(framebufs.size()))},
      graphics_queue{device.getQueue(queue_indices.graphics_queue, 0)},
      rendered_semaphores{create_semaphores(device, uint32_t(framebufs.size()))},
      presentation_queue{device.getQueue(queue_indices.presentation_queue, 0)},
      rendered_fences{create_fences(device, true, uint32_t(framebufs.size()))} {}

void render_proc::render_begin(const render_target &rt) {
    current_img_index = rt.acquire_frame(device);
    const auto &cmd_buf = draw_cmd_buf[current_img_index].get();
    const auto &fence = rendered_fences[current_img_index].get();

    device.waitForFences({fence}, VK_TRUE, UINT64_MAX);

    cmd_buf.reset();
    device.resetFences({fence});

    vk::CommandBufferBeginInfo cmdBeginInfo;
    cmd_buf.begin(cmdBeginInfo);

    vk::RenderPassBeginInfo renderpassBeginInfo;
    renderpassBeginInfo.renderPass = renderpass.get();
    renderpassBeginInfo.framebuffer = framebufs[current_img_index].get();
    renderpassBeginInfo.renderArea = vk::Rect2D({0, 0}, rt.extent());
    renderpassBeginInfo.clearValueCount = 0;
    renderpassBeginInfo.pClearValues = nullptr;

    cmd_buf.beginRenderPass(renderpassBeginInfo, vk::SubpassContents::eInline);

    cmd_buf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
}
void render_proc::render_end(const render_target &rt) {
    const auto &cmd_buf = draw_cmd_buf[current_img_index].get();
    const auto &rendered_semaphore = rendered_semaphores[current_img_index].get();

    cmd_buf.endRenderPass();
    cmd_buf.end();

    {
        vk::SubmitInfo submitInfo;

        auto submit_cmd_buf = {cmd_buf};
        submitInfo.commandBufferCount = uint32_t(submit_cmd_buf.size());
        submitInfo.pCommandBuffers = submit_cmd_buf.begin();

        auto render_signal_semaphores = {rendered_semaphore};
        submitInfo.signalSemaphoreCount = uint32_t(render_signal_semaphores.size());
        submitInfo.pSignalSemaphores = render_signal_semaphores.begin();

        vk::Semaphore renderwaitSemaphores[] = {rt.image_prepared_semaphore()};
        vk::PipelineStageFlags renderwaitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = renderwaitSemaphores;
        submitInfo.pWaitDstStageMask = renderwaitStages;

        graphics_queue.submit({submitInfo}, rendered_fences[current_img_index].get());
    }

    rt.present(presentation_queue, current_img_index, std::array{rendered_semaphore});
}

void render_proc::draw_rect(const render_target &rt, float x, float y, float w, float h, float theta, float ax, float ay) {
    const auto &cmd_buf = draw_cmd_buf[current_img_index].get();

    const auto
        cos_th = cosf(theta),
        sin_th = sinf(theta);

    mat3 pivot_mat{.m{
        {1.0f, 0.0f, -ax},
        {0.0f, 1.0f, -ay},
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

    const shader_pushconstant data{
        .draw_matrix{move_mat * rotate_mat * scale_mat * pivot_mat},
        .screen_size{
            .v{float(rt.extent().width), float(rt.extent().height)},
        },
    };
    cmd_buf.pushConstants<shader_pushconstant>(pipeline_layout.get(), vk::ShaderStageFlagBits::eVertex, 0, {data});
    cmd_buf.draw(4, 1, 0, 0);
}
