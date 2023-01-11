// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module App;

import ShaderData;
#else
    #include "App.mpp"
#endif

using namespace stormkit;

////////////////////////////////////////
////////////////////////////////////////
App::App() {
    doInitWindow();
    doInitBaseRenderObjects();
    doInitMeshRenderObjects();
}

////////////////////////////////////////
////////////////////////////////////////
App::~App() {
    ilog("Cleaning");

    m_device->waitIdle();
}

////////////////////////////////////////
////////////////////////////////////////
auto App::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> core::Int32 {
    namespace Chrono = std::chrono;
    using Clock      = std::chrono::high_resolution_clock;

    const auto extent = wsi::Window::getPrimaryMonitorSettings().extents.back();

    auto event_handler = wsi::EventHandler { *m_window };
    event_handler.addCallback(wsi::EventType::Closed,
                              [this]([[maybe_unused]] const wsi::Event& event) {
                                  m_window->close();
                              });
    event_handler.addCallback(wsi::EventType::KeyReleased,
                              [&]([[maybe_unused]] const wsi::Event& event) {
                                  const auto& event_data =
                                      core::as<wsi::KeyReleasedEventData>(event.data);
                                  if (event_data.key == wsi::Key::Escape) [[unlikely]]
                                      m_window->close();
                                  if (event_data.key == wsi::Key::F11) {
                                      if (m_fullscreen_enabled) {
                                          m_fullscreen_enabled = false;
                                          m_window->setFullscreenEnabled(false);
                                      } else {
                                          m_fullscreen_enabled = true;
                                          m_window->setExtent(extent);
                                          m_window->setFullscreenEnabled(true);
                                      }
                                  }
                              });

    auto start_time = Clock::now();
    while (m_window->isOpen()) {
        event_handler.update();

        if (m_surface->needRecreate()) {
            m_surface->recreate();
            doInitPerFrameObjects();
        }

        const auto now_timepoint = Clock::now();
        const auto delta         = core::Secondf { now_timepoint - start_time }.count();

        m_model.model = core::rotate(core::math::Matrix { 1.f },
                                     delta * core::radians(90.f),
                                     core::math::Vector3f { 0.f, 0.f, 1.f });

        auto frame = std::move(m_surface->acquireNextFrame().value());

        m_model_staging_buffer->upload<Transform>({ &m_model, 1 },
                                                  MODEL_BUFFER_SIZE * frame.image_index);
        m_model_staging_buffer->flush(MODEL_BUFFER_SIZE * frame.image_index, MODEL_BUFFER_SIZE);

        auto& frame_data = m_frame_datas[frame.image_index];

        auto wait   = core::makeConstObserverStaticArray(frame.image_available);
        auto signal = core::makeConstObserverStaticArray(frame.render_finished);

        frame_data.commandbuffer.submit(wait, signal, frame.in_flight);

        m_surface->present(frame);
    }

    return EXIT_SUCCESS;
}

////////////////////////////////////////
////////////////////////////////////////
auto App::doInitWindow() -> void {
    // First we create the wsi
    const auto window_style = wsi::WindowStyle::All;

    m_window = std::make_unique<wsi::Window>(WINDOW_TITLE,
                                             core::math::ExtentU { 800u, 600u },
                                             window_style);
}

////////////////////////////////////////
////////////////////////////////////////
auto App::doInitBaseRenderObjects() -> void {
    // We create an instance and initialize device on best available GPU
    m_instance = std::make_unique<gpu::Instance>();
    ilog("Render backend successfully initialized");
    ilog("Using StormKit {}.{}.{} (branch: {}, commit_hash: {}), built with {}",
         core::STORMKIT_MAJOR_VERSION,
         core::STORMKIT_MINOR_VERSION,
         core::STORMKIT_PATCH_VERSION,
         core::STORMKIT_GIT_BRANCH,
         core::STORMKIT_GIT_COMMIT_HASH,
         STORMKIT_COMPILER);

    ilog("--------- Physical Devices ----------");
    for (const auto& device : m_instance->physicalDevices()) ilog("{}", device.info());

    auto surface = m_instance->allocateWindowSurface(*m_window);

    const auto& physical_device = m_instance->pickPhysicalDevice(*surface);

    m_surface = std::move(surface);

    const auto& physical_device_info = physical_device.info();

    ilog("Using physical device {} ({:#06x})",
         physical_device_info.device_name,
         physical_device_info.device_id);

    m_device = physical_device.allocateLogicalDevice();
    ilog("Device successfully created");

    // We initialize the surface (it will acquire images and init all surface default parameters
    m_surface->initialize(*m_device);
    ilog("Device successfully initialized with {} image(s)", m_surface->imageCount());

    m_queue = core::makeConstObserver(m_device->graphicsQueue());
}

////////////////////////////////////////
////////////////////////////////////////
auto App::doInitMeshRenderObjects() -> void {
    const auto& surface_extent = m_surface->extent();
    const auto surface_extentf = core::math::ExtentF { surface_extent };
    const auto buffering_count = m_surface->bufferingCount();

    // We initialize the camera et set the model matrix to default
    m_camera      = { .projection = core::perspective(core::radians(45.f),
                                                 surface_extentf.width / surface_extentf.height,
                                                 0.1f,
                                                 10.f),
                      .view       = core::lookAt(core::math::Vector3f { 4.f, 3.f, 3.f },
                                                 { 0.f, 0.f, 0.f },
                                                 { 0.f, 1.f, 0.f }) };
    m_model.model = core::math::Matrix { 1.f };

    // We load our triangle shaders
    m_vertex_shader = m_device->allocateShader(SHADER_DATA, gpu::ShaderStageFlag::Vertex);
    ilog("Vertex shader loaded");
    m_fragment_shader = m_device->allocateShader(SHADER_DATA, gpu::ShaderStageFlag::Fragment);
    ilog("Fragment shader loaded");

    // We need to indicate to the pipeline how we pass data to the shader, first we need a
    // descriptor set layout
    m_descriptor_set_layout = m_device->allocateDescriptorSetLayout();
    m_descriptor_set_layout->addBinding({ .binding          = 0,
                                          .type             = gpu::DescriptorType::Uniform_Buffer,
                                          .stages           = gpu::ShaderStageFlag::Vertex,
                                          .descriptor_count = 1 });
    m_descriptor_set_layout->addBinding({ .binding = 1,
                                          .type    = gpu::DescriptorType::Uniform_Buffer_Dynamic,
                                          .stages  = gpu::ShaderStageFlag::Vertex,
                                          .descriptor_count = 1 });
    m_descriptor_set_layout->bake();

    // Next we need a pool to allocate descriptors
    m_descriptor_pool = m_device->allocateDescriptorPool(
        std::array { gpu::DescriptorPool::Size { gpu::DescriptorType::Uniform_Buffer, 1 },
                     gpu::DescriptorPool::Size { gpu::DescriptorType::Uniform_Buffer_Dynamic, 1 } },
        1);

    // We need to create a render pass, the render pass describe how the framebuffer will look
    const auto description = gpu::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() },
                         { .format = gpu::PixelFormat::Depth32F_Stencil8,
                           .destination_layout =
                               gpu::ImageLayout::Depth_Stencil_Attachment_Optimal }

        },
        .subpasses   = { { .bind_point      = gpu::PipelineBindPoint::Graphics,
                           .attachment_refs = { { .attachment_id = 0u },
                                                { .attachment_id = 1u,
                                                  .layout        = gpu::ImageLayout::
                                                      Depth_Stencil_Attachment_Optimal } } } }
    };
    m_render_pass = m_device->allocateRenderPass(description);
    ilog("Renderpass successfully created");

    // We create a pipeline, the pipeline describe all the fixed function parameters and the shaders
    // wich will be bound
    m_pipeline       = m_device->allocateRasterPipeline();
    const auto state = gpu::RasterPipelineState {
        .viewport_state = { .viewports = { gpu::Viewport { .position = { 0.f, 0.f },
                                                           .extent   = surface_extentf,
                                                           .depth    = { 0.f, 1.f } } },
                            .scissors  = { gpu::Scissor { .offset = { 0, 0 },
                                                          .extent = surface_extent } } },

        .color_blend_state   = { .attachments = { {} } },
        .dynamic_state       = { { gpu::DynamicState::Viewport, gpu::DynamicState::Scissor } },
        .shader_state        = { .shaders =
                                     core::makeConstObserverArray(m_vertex_shader, m_fragment_shader) },
        .vertex_input_state  = { .binding_descriptions =
                                     core::toArray(MESH_VERTEX_BINDING_DESCRIPTIONS),
                                 .input_attribute_descriptions =
                                     core::toArray(MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS) },
        .depth_stencil_state = { .depth_test_enable  = true,
                                 .depth_write_enable = true,
                                 .depth_compare_op   = gpu::CompareOperation::Greater },
        .layout              = { .descriptor_set_layouts =
                                     core::makeConstObserverArray(m_descriptor_set_layout) }
    };

    m_pipeline->setState(state);
    m_pipeline->setRenderPass(*m_render_pass);
    m_pipeline->bake();
    ilog("engine pipeline successfully created");

    const auto& capabilities = m_device->physicalDevice().capabilities();
    auto cmb                 = m_queue->createCommandBuffer();

    cmb.begin(true);

    auto staging_buffer =
        m_device->createStagingBuffer(MESH_VERTEX_BUFFER_SIZE + CAMERA_BUFFER_SIZE);
    staging_buffer.upload<Vertex>(MESH_VERTICES);
    ilog("{} bytes uploaded to vertex buffer", MESH_VERTEX_BUFFER_SIZE);
    staging_buffer.upload<Camera>({ &m_camera, 1 }, MESH_VERTEX_BUFFER_SIZE);
    ilog("{} bytes uploaded to camera buffer", CAMERA_BUFFER_SIZE);
    staging_buffer.flush(0, MESH_VERTEX_BUFFER_SIZE + CAMERA_BUFFER_SIZE);

    // We create our mesh data, we only need a vertex buffer
    m_vertex_buffer = m_device->allocateVertexBuffer(MESH_VERTEX_BUFFER_SIZE);
    cmb.copyBuffer(staging_buffer, *m_vertex_buffer, MESH_VERTEX_BUFFER_SIZE);

    // We allocate the set and prepare data
    m_descriptor_set = m_descriptor_pool->allocateDescriptorSet(*m_descriptor_set_layout);

    m_camera_buffer = m_device->allocateUniformBuffer(
        gpu::computeUniformBufferOffsetAlignement(CAMERA_BUFFER_SIZE, capabilities),
        gpu::MemoryPropertyFlag::Device_Local);
    cmb.copyBuffer(staging_buffer, *m_camera_buffer, CAMERA_BUFFER_SIZE, MESH_VERTEX_BUFFER_SIZE);

    m_model_staging_buffer = m_device->allocateStagingBuffer(MODEL_BUFFER_SIZE * buffering_count);
    for (auto i : core::range(buffering_count))
        m_model_staging_buffer->upload<Transform>({ &m_model, 1 }, i * MODEL_BUFFER_SIZE);
    m_model_staging_buffer->flush(0, MODEL_BUFFER_SIZE * buffering_count);

    m_model_buffer = m_device->allocateUniformBuffer(
        gpu::computeUniformBufferOffsetAlignement(MODEL_BUFFER_SIZE * buffering_count,
                                                  capabilities),
        gpu::MemoryPropertyFlag::Device_Local);

    cmb.end();

    auto fence = m_device->createFence();
    cmb.submit({}, {}, core::makeObserver(fence));

    const auto descriptors = core::makeStaticArray(
        gpu::Descriptor { gpu::BufferDescriptor {
            .binding = 0,
            .buffer  = core::makeConstObserver(m_camera_buffer),
            .range   = core::as<core::UInt32>(
                gpu::computeUniformBufferOffsetAlignement(CAMERA_BUFFER_SIZE, capabilities)) } },
        gpu::Descriptor { gpu::BufferDescriptor {
            .type    = gpu::DescriptorType::Uniform_Buffer_Dynamic,
            .binding = 1,
            .buffer  = core::makeConstObserver(m_model_buffer),
            .range   = core::as<core::UInt32>(
                gpu::computeUniformBufferOffsetAlignement(MODEL_BUFFER_SIZE, capabilities)) } });
    m_descriptor_set->update(descriptors);
    // Finally we create per frame data, we need a commandbuffer and a framebuffer by frame to avoid
    // waiting the GPU
    doInitPerFrameObjects();

    fence.wait();
}

////////////////////////////////////////
////////////////////////////////////////
auto App::doInitPerFrameObjects() -> void {
    const auto& surface_extent = m_surface->extent();
    const auto surface_extentf = core::math::ExtentF { surface_extent };
    const auto buffering_count = m_surface->bufferingCount();

    m_camera.projection = core::perspective(core::radians(45.f),
                                            surface_extentf.width / surface_extentf.height,
                                            0.1f,
                                            10.f);

    m_surface_views.clear();
    m_surface_views.reserve(std::size(m_surface->images()));
    for (const auto& image : m_surface->images()) m_surface_views.emplace_back(image.createView());

    m_depth_buffers.reserve(buffering_count);
    m_depth_buffer_views.reserve(buffering_count);

    m_frame_datas.clear();
    m_frame_datas.reserve(buffering_count);

    auto vertex_buffers = core::makeConstObserverStaticArray(m_vertex_buffer);
    auto vertex_offsets = std::array { core::UInt64 { 0 } };

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

    const auto clear_values = core::makeStaticArray(gpu::ClearValue { gpu::ClearColor {} },
                                                    gpu::ClearValue { gpu::ClearDepthStencil {} });

    for (auto i : core::range(buffering_count)) {
        const auto& image_view = m_surface_views[i];

        const auto& depth =
            m_depth_buffers.emplace_back(*m_device,
                                         gpu::Image::CreateInfo {
                                             surface_extent,
                                             gpu::PixelFormat::Depth32F_Stencil8,
                                             1u,
                                             1u,
                                             gpu::ImageType::T2D,
                                             gpu::ImageCreateFlag::None,
                                             gpu::SampleCountFlag::C1,
                                             gpu::ImageUsageFlag::Depth_Stencil_Attachment });

        const auto& depth_view = m_depth_buffer_views.emplace_back(
            depth.createView(gpu::ImageViewType::T2D,
                             { .aspect_mask = gpu::ImageAspectMaskFlag::Depth }));

        const auto attachments = core::makeConstRefArray(image_view, depth_view);

        auto& frame = m_frame_datas.emplace_back(Frame {
            .framebuffer = m_render_pass->createFrameBuffer(surface_extent, std::move(attachments)),
            .commandbuffer = m_queue->createCommandBuffer(),
        });

        auto sets    = core::makeConstObserverStaticArray(m_descriptor_set);
        auto offsets = std::array { core::as<core::UInt32>(i * MODEL_BUFFER_SIZE) };

        frame.commandbuffer.begin();
        frame.commandbuffer.copyBuffer(*m_model_staging_buffer,
                                       *m_model_buffer,
                                       MODEL_BUFFER_SIZE,
                                       i * MODEL_BUFFER_SIZE,
                                       i * MODEL_BUFFER_SIZE);
        frame.commandbuffer.beginRenderPass(*m_render_pass, frame.framebuffer, clear_values);
        frame.commandbuffer.bindRasterPipeline(*m_pipeline);
        frame.commandbuffer.setViewport(0, viewports);
        frame.commandbuffer.setScissor(0, scissors);
        frame.commandbuffer.bindDescriptorSets(*m_pipeline, sets, offsets);
        frame.commandbuffer.bindVertexBuffers(vertex_buffers, vertex_offsets);

        frame.commandbuffer.draw(core::as<core::Int32>(std::size(MESH_VERTICES)));

        frame.commandbuffer.endRenderPass();
        frame.commandbuffer.end();

        ilog("Frame {} CommandBuffer and FrameBuffer successfully created", i);
    }
}

////////////////////////////////////////
////////////////////////////////////////
auto App::updateModelMatrix([[maybe_unused]] stormkit::core::Secondf& delta) -> void {
}
