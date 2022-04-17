#include "App.mpp"
#include "Constants.mpp"

#include <stormkit/core/Configure.mpp>

#include <stormkit/log/LogHandler.mpp>

#include <stormkit/wsi/Event.mpp>
#include <stormkit/wsi/EventHandler.mpp>
#include <stormkit/wsi/VideoSettings.mpp>
#include <stormkit/wsi/Window.mpp>

#include <stormkit/render/core/Device.mpp>
#include <stormkit/render/core/Instance.mpp>
#include <stormkit/render/core/PhysicalDevice.mpp>
#include <stormkit/render/core/PhysicalDeviceInfo.mpp>
#include <stormkit/render/core/Queue.mpp>
#include <stormkit/render/core/WindowSurface.mpp>

using namespace stormkit;

App::App() {
    doInitWindow();
    doInitBaseRenderObjects();
    doInitMeshRenderObjects();
}

App::~App() {
    ilog("Cleaning");

    m_device->waitIdle();
}

auto App::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> core::Int32 {
    auto event_handler = wsi::EventHandler { *m_window };
    event_handler.addCallback(wsi::EventType::Closed,
                              [this]([[maybe_unused]] const wsi::Event &event) {
                                  m_window->close();
                              });
    event_handler.addCallback(wsi::EventType::KeyReleased,
                              [this]([[maybe_unused]] const wsi::Event &event) {
                                  const auto &event_data =
                                      core::as<wsi::KeyReleasedEventData>(event.data);
                                  if (event_data.key == wsi::Key::Escape) [[unlikely]]
                                      m_window->close();
                              });

    while (m_window->isOpen()) {
        event_handler.update();
        auto frame       = m_surface->acquireNextFrame();
        auto &frame_data = m_frame_datas[frame.image_index % m_surface->bufferingCount()];

        auto wait   = std::array { frame.image_available };
        auto signal = std::array { frame.render_finished };

        frame_data.commandbuffer.submit(wait, signal, frame.in_flight);

        m_surface->present(frame);
    }

    return EXIT_SUCCESS;
}

void App::doInitWindow() {
    // First we create the wsi
    const auto window_style = wsi::WindowStyle::Close;

    m_window = std::make_unique<wsi::Window>(WINDOW_TITLE,
                                             wsi::VideoSettings { .size = { 800u, 600u } },
                                             window_style);
}

void App::doInitBaseRenderObjects() {
    // We create an instance and initialize device on best available GPU
    m_instance = std::make_unique<render::Instance>();
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

    m_surface = m_instance->allocateWindowSurface(*m_window);

    const auto &physical_device      = m_instance->pickPhysicalDevice(*m_surface);
    const auto &physical_device_info = physical_device.info();

    ilog("Using physical device {}", physical_device_info.device_name);
    ilog("{}", physical_device_info);

    m_device = physical_device.allocateLogicalDevice();
    ilog("Device successfully created");

    // We initialize the surface (it will acquire images and init all surface default parameters
    m_surface->initialize(*m_device);
    ilog("Device successfully initialized with {} image(s)", m_surface->imageCount());

    m_queue = core::makeConstObserver(m_device->graphicsQueue());

    for (const auto &image : m_surface->images()) m_surface_views.emplace_back(image.createView());
}

void App::doInitMeshRenderObjects() {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };
    const auto buffering_count = m_surface->bufferingCount();

    // We load our triangle shaders
    m_vertex_shader = m_device->allocateShader(VERTEX_SHADER_DATA, render::ShaderStageFlag::Vertex);
    ilog("Vertex shader loaded");
    m_fragment_shader =
        m_device->allocateShader(FRAGMENT_SHADER_DATA, render::ShaderStageFlag::Fragment);
    ilog("Fragment shader loaded");

    // We need to create a render pass, the render pass describe how the framebuffer will look
    const auto description = render::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() } },
        .subpasses   = { { .bind_point      = render::PipelineBindPoint::Graphics,
                           .attachment_refs = { { .attachment_id = 0u } } } }
    };
    m_render_pass = m_device->allocateRenderPass(description);
    ilog("Renderpass successfully created");

    // We create a pipeline, the pipeline describe all the fixed function parameters and the shaders
    // wich will be bound
    m_pipeline       = m_device->allocateGraphicsPipeline();
    const auto state = render::GraphicsPipelineState {
        .viewport_state     = { .viewports = { render::Viewport { .position = { 0.f, 0.f },
                                                                  .extent   = surface_extentf,
                                                                  .depth    = { 0.f, 1.f } } },
                                .scissors  = { render::Scissor { .offset = { 0, 0 },
                                                                 .extent = surface_extent } } },
        .color_blend_state  = { .attachments = { {} } },
        .shader_state       = { .shaders =
                              core::makeConstObserverArray(m_vertex_shader, m_fragment_shader) },
        .vertex_input_state = { .binding_descriptions =
                                    core::toArray(MESH_VERTEX_BINDING_DESCRIPTIONS),
                                .input_attribute_descriptions =
                                    core::toArray(MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS) },
    };

    m_pipeline->setState(state);
    m_pipeline->setRenderPass(*m_render_pass);
    m_pipeline->bake();
    ilog("engine pipeline successfully created");

    // We create our mesh data, we only need a vertex buffer
    m_vertex_buffer = m_device->allocateVertexBuffer(MESH_VERTEX_BUFFER_SIZE);
    m_vertex_buffer->upload<Vertex>(MESH_VERTICES);
    m_vertex_buffer->flush(0, MESH_VERTEX_BUFFER_SIZE);
    ilog("{} bytes uploaded to vertex buffer", MESH_VERTEX_BUFFER_SIZE);

    // Finally we create per frame data, we need a commandbuffer and a framebuffer by frame to avoid
    // waiting the GPU
    m_frame_datas.reserve(buffering_count);

    auto vertex_buffers = core::makeConstObserverStaticArray(m_vertex_buffer);
    auto offsets        = std::array { core::UInt64 { 0 } };
    for (auto i = 0u; i < buffering_count; ++i) {
        const auto &image_view = m_surface_views[i];

        auto frame =
            Frame { .framebuffer =
                        m_render_pass->createFramebuffer(surface_extent,
                                                         core::makeConstRefArray(image_view)),
                    .commandbuffer = m_queue->createCommandBuffer() };

        frame.commandbuffer.begin();
        frame.commandbuffer.beginRenderPass(*m_render_pass, frame.framebuffer);
        frame.commandbuffer.bindGraphicsPipeline(*m_pipeline);
        frame.commandbuffer.bindVertexBuffers(vertex_buffers, offsets);

        frame.commandbuffer.draw(core::as<core::Int32>(std::size(MESH_VERTICES)));

        frame.commandbuffer.endRenderPass();
        frame.commandbuffer.end();

        m_frame_datas.emplace_back(std::move(frame));
        ilog("Frame {} CommandBuffer and Framebuffer successfully created", i);
    }
}
