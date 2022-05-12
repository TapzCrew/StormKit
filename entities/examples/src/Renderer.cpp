
#include "Renderer.mpp"
#include "Constants.mpp"

#include <stormkit/core/Configure.mpp>

#include <stormkit/log/LogHandler.mpp>

#include <stormkit/wsi/Event.mpp>
#include <stormkit/wsi/EventHandler.mpp>
#include <stormkit/wsi/Monitor.mpp>
#include <stormkit/wsi/Window.mpp>

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Instance.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/PhysicalDeviceInfo.mpp>
#include <stormkit/gpu/core/Queue.mpp>
#include <stormkit/gpu/core/WindowSurface.mpp>

using namespace stormkit;

Renderer::Renderer(const wsi::Window &window) : m_window { &window } {
    doInitBaseRenderObjects();
    doInitMeshRenderObjects();
}

Renderer::~Renderer() {
    m_device->waitIdle();
}

Renderer::Renderer(Renderer &&) noexcept = default;

auto Renderer::operator=(Renderer &&) noexcept -> Renderer & = default;

auto Renderer::renderFrame() -> void {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };

    if (m_surface->needRecreate()) {
        m_surface->recreate();
        doInitPerFrameObjects();
    }

    const auto viewports = [&] {
        auto v = std::vector<gpu::Viewport> {};
        v.emplace_back(gpu::Viewport { .extent = surface_extentf, .depth = { 0, 1 } });

        return v;
    }();

    const auto scissors = [&] {
        auto s = std::vector<gpu::Scissor> {};
        s.emplace_back(gpu::Scissor { .extent = surface_extent });

        return s;
    }();

    auto sets = core::makeConstObserverStaticArray(m_board.descriptor_set);

    auto frame          = std::move(m_surface->acquireNextFrame().value());
    auto &framebuffer   = m_framebuffers[frame.image_index];
    auto &commandbuffer = m_command_buffers[frame.image_index];

    auto wait   = core::makeConstObserverStaticArray(frame.image_available);
    auto signal = core::makeConstObserverStaticArray(frame.render_finished);

    m_current_fence = frame.in_flight;

    commandbuffer.reset();
    commandbuffer.begin();
    commandbuffer.beginRenderPass(*m_render_pass, framebuffer);
    commandbuffer.bindGraphicsPipeline(*m_board.pipeline);
    commandbuffer.setViewport(0, viewports);
    commandbuffer.setScissor(0, scissors);
    commandbuffer.bindDescriptorSets(*m_board.pipeline, sets, {});

    commandbuffer.draw(4);

    commandbuffer.endRenderPass();
    commandbuffer.end();
    commandbuffer.submit(wait, signal, frame.in_flight);

    m_surface->present(frame);
}

auto Renderer::updateBoard(const stormkit::image::Image &board) -> void {
    m_board.current_image += 1;
    m_board.current_image %= std::size(m_board.images);

    m_board.images[m_board.current_image].loadFromImage(board);

    if (m_current_fence) m_current_fence->wait();

    const auto descriptors = core::makeStaticArray(gpu::Descriptor { gpu::ImageDescriptor {
        .type       = gpu::DescriptorType::Combined_Image_Sampler,
        .binding    = 0,
        .layout     = gpu::ImageLayout::Shader_Read_Only_Optimal,
        .image_view = core::makeConstObserver(m_board.image_views[m_board.current_image]),
        .sampler    = core::makeConstObserver(m_board.sampler) } });
    m_board.descriptor_set->update(descriptors);
}

auto Renderer::doInitBaseRenderObjects() -> void {
    // We create an instance and initialize device on best available GPU
    m_instance = std::make_unique<gpu::Instance>();
    ilog("Render backend successfully initialized");
    ilog("Using StormKit {}.{}.{} {} {}, built with {}",
         core::STORMKIT_MAJOR_VERSION,
         core::STORMKIT_MINOR_VERSION,
         core::STORMKIT_PATCH_VERSION,
         core::STORMKIT_GIT_BRANCH,
         core::STORMKIT_GIT_COMMIT_HASH,
         STORMKIT_COMPILER);

    ilog("--------- Physical Devices ----------");
    for (const auto &device : m_instance->physicalDevices()) ilog("{}", device.info());

    auto surface = m_instance->allocateWindowSurface(*m_window);

    const auto &physical_device = m_instance->pickPhysicalDevice(*surface);

    m_surface = std::move(surface);

    const auto &physical_device_info = physical_device.info();

    ilog("Using physical device {} ({:#06x})",
         physical_device_info.device_name,
         physical_device_info.device_id);

    m_device = physical_device.allocateLogicalDevice();

    m_surface->initialize(*m_device);

    m_queue           = core::makeConstObserver(m_device->graphicsQueue());
    m_command_buffers = m_queue->createCommandBuffers(m_surface->bufferingCount());
}

auto Renderer::doInitMeshRenderObjects() -> void {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };

    m_board.vertex_shader =
        m_device->allocateShader(VERTEX_SHADER_DATA, gpu::ShaderStageFlag::Vertex);
    m_board.fragment_shader =
        m_device->allocateShader(FRAGMENT_SHADER_DATA, gpu::ShaderStageFlag::Fragment);

    const auto description = gpu::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() } },
        .subpasses   = { { .bind_point      = gpu::PipelineBindPoint::Graphics,
                           .attachment_refs = { { .attachment_id = 0u } } } }
    };

    m_descriptor_set_layout = m_device->allocateDescriptorSetLayout();
    m_descriptor_set_layout->addBinding({ .binding = 0,
                                          .type    = gpu::DescriptorType::Combined_Image_Sampler,
                                          .stages  = gpu::ShaderStageFlag::Fragment,
                                          .descriptor_count = 1 });
    m_descriptor_set_layout->bake();
    m_descriptor_pool =
        m_device->allocateDescriptorPool(std::array { gpu::DescriptorPool::Size {
                                             gpu::DescriptorType::Combined_Image_Sampler,
                                             1 } },
                                         1);

    m_render_pass = m_device->allocateRenderPass(description);

    m_board.pipeline = m_device->allocateGraphicsPipeline();
    const auto state = gpu::GraphicsPipelineState {
        .input_assembly_state = { .topology = gpu::PrimitiveTopology::Triangle_Strip },
        .viewport_state       = { .viewports = { gpu::Viewport { .position = { 0.f, 0.f },
                                                                 .extent   = surface_extentf,
                                                                 .depth    = { 0.f, 1.f } } },
                                  .scissors  = { gpu::Scissor { .offset = { 0, 0 },
                                                                .extent = surface_extent } } },
        .color_blend_state    = { .attachments = { { .blend_enable = true,
                                                     .src_color_blend_factor =
                                                      gpu::BlendFactor::Src_Alpha,
                                                     .dst_color_blend_factor =
                                                      gpu::BlendFactor::One_Minus_Src_Alpha,
                                                     .src_alpha_blend_factor =
                                                      gpu::BlendFactor::Src_Alpha,
                                                     .dst_alpha_blend_factor =
                                                      gpu::BlendFactor::One_Minus_Src_Alpha,
                                                     .alpha_blend_operation =
                                                      gpu::BlendOperation::Add } } },
        .dynamic_state        = { { gpu::DynamicState::Viewport, gpu::DynamicState::Scissor } },
        .shader_state         = { .shaders = core::makeConstObserverArray(m_board.vertex_shader,
                                                                  m_board.fragment_shader) },
        /*.vertex_input_state   = { .binding_descriptions =
                                    core::toArray(MESH_VERTEX_BINDING_DESCRIPTIONS),
                                  .input_attribute_descriptions =
                                    core::toArray(MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS) },*/
        .layout = { .descriptor_set_layouts =
                        core::makeConstObserverArray(m_descriptor_set_layout) }

    };

    m_board.pipeline->setState(state);
    m_board.pipeline->setRenderPass(*m_render_pass);
    m_board.pipeline->bake();

    for (auto i : core::range(BOARD_BUFFERING_COUNT)) {
        auto &img = m_board.images.emplace_back(*m_device,
                                                gpu::Image::CreateInfo {
                                                    .extent = { BOARD_SIZE, BOARD_SIZE } });

        m_board.image_views.emplace_back(img.createView());
    }

    m_board.sampler = m_device->allocateSampler(
        gpu::Sampler::Settings { .mag_filter     = gpu::Filter::Nearest,
                                 .min_filter     = gpu::Filter::Nearest,
                                 .address_mode_u = gpu::SamplerAddressMode::Clamp_To_Edge,
                                 .address_mode_v = gpu::SamplerAddressMode::Clamp_To_Edge });

    m_board.descriptor_set = m_descriptor_pool->allocateDescriptorSet(*m_descriptor_set_layout);

    doInitPerFrameObjects();
}

auto Renderer::doInitPerFrameObjects() -> void {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };
    const auto buffering_count = m_surface->bufferingCount();

    m_surface_views.clear();
    m_surface_views.reserve(std::size(m_surface->images()));
    for (const auto &image : m_surface->images()) m_surface_views.emplace_back(image.createView());

    m_framebuffers.clear();
    m_framebuffers.reserve(buffering_count);

    for (auto i : core::range(buffering_count)) {
        const auto &image_view = m_surface_views[i];
        auto attachments       = core::makeConstRefArray(image_view);

        m_framebuffers.emplace_back(*m_render_pass, surface_extent, std::move(attachments));
    }
}
