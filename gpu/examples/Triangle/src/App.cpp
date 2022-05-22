#include "App.mpp"
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
    const auto size = wsi::Window::getPrimaryMonitorSettings().sizes.back();

    auto stop = std::atomic_bool { false };

    auto event_handler = wsi::EventHandler { *m_window };
    event_handler.addCallback(wsi::EventType::Closed,
                              [this]([[maybe_unused]] const wsi::Event &event) {
                                  m_window->close();
                              });
    event_handler.addCallback(wsi::EventType::KeyReleased,
                              [&]([[maybe_unused]] const wsi::Event &event) {
                                  const auto &event_data =
                                      core::as<wsi::KeyReleasedEventData>(event.data);
                                  if (event_data.key == wsi::Key::Escape) [[unlikely]] {
                                      stop = true;
                                      m_window->close();
                                  }
                                  if (event_data.key == wsi::Key::F11) {
                                      if (m_fullscreen_enabled) {
                                          m_fullscreen_enabled = false;
                                          m_window->setFullscreenEnabled(false);
                                      } else {
                                          m_fullscreen_enabled = true;
                                          m_window->setSize(size);
                                          m_window->setFullscreenEnabled(true);
                                      }
                                  }
                              });

    while (m_window->isOpen()) {
        event_handler.update();
        if (m_surface->needRecreate()) {
            m_surface->recreate();
            doInitPerFrameObjects();
        }

        auto frame       = std::move(m_surface->acquireNextFrame().value());
        auto &frame_data = m_frame_datas[frame.image_index];

        auto wait   = core::makeConstObserverStaticArray(frame.image_available);
        auto signal = core::makeConstObserverStaticArray(frame.render_finished);

        frame_data.commandbuffer.submit(wait, signal, frame.in_flight);

        m_surface->present(frame);
    }

    return EXIT_SUCCESS;
}

auto App::doInitWindow() -> void {
    // First we create the wsi
    const auto window_style = wsi::WindowStyle::All;

    m_window =
        std::make_unique<wsi::Window>(WINDOW_TITLE, core::ExtentU { 800u, 600u }, window_style);
}

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
    for (const auto &device : m_instance->physicalDevices()) ilog("{}", device.info());

    auto surface = m_instance->allocateWindowSurface(*m_window);

    const auto &physical_device = m_instance->pickPhysicalDevice(*surface);

    m_surface = std::move(surface);

    const auto &physical_device_info = physical_device.info();

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

auto App::doInitMeshRenderObjects() -> void {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };

    // We load our triangle shaders
    m_vertex_shader = m_device->allocateShader(SHADER_DATA, gpu::ShaderStageFlag::Vertex);
    ilog("Vertex shader loaded");
    m_fragment_shader = m_device->allocateShader(SHADER_DATA, gpu::ShaderStageFlag::Fragment);
    ilog("Fragment shader loaded");

    // We need to create a render pass, the render pass describe how the framebuffer will look
    const auto description = gpu::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() } },
        .subpasses   = { { .bind_point      = gpu::PipelineBindPoint::Graphics,
                           .attachment_refs = { { .attachment_id = 0u } } } }
    };
    m_render_pass = m_device->allocateRenderPass(description);
    ilog("Renderpass successfully created");

    // We create a pipeline, the pipeline describe all the fixed function parameters and the shaders
    // wich will be bound
    m_pipeline       = m_device->allocateGraphicsPipeline();
    const auto state = gpu::GraphicsPipelineState {
        .viewport_state = { .viewports = { gpu::Viewport { .position = { 0.f, 0.f },
                                                           .extent   = surface_extentf,
                                                           .depth    = { 0.f, 1.f } } },
                            .scissors  = { gpu::Scissor { .offset = { 0, 0 },
                                                          .extent = surface_extent } } },

        .color_blend_state  = { .attachments = { {} } },
        .dynamic_state      = { { gpu::DynamicState::Viewport, gpu::DynamicState::Scissor } },
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

    // Finally we create per frame data, we need a commandbuffer and a framebuffer by frame to avoid
    // waiting the GPU
    doInitPerFrameObjects();
}

auto App::doInitPerFrameObjects() -> void {
    const auto &surface_extent = m_surface->extent();
    const auto surface_extentf = core::ExtentF { surface_extent };
    const auto buffering_count = m_surface->bufferingCount();

    m_surface_views.clear();
    m_surface_views.reserve(std::size(m_surface->images()));
    for (const auto &image : m_surface->images()) m_surface_views.emplace_back(image.createView());

    m_frame_datas.clear();
    m_frame_datas.reserve(buffering_count);

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

    for (auto i : core::range(buffering_count)) {
        const auto &image_view = m_surface_views[i];
        auto attachments       = core::makeConstRefArray(image_view);

        auto frame =
            Frame { .framebuffer =
                        m_render_pass->createFramebuffer(surface_extent, std::move(attachments)),
                    .commandbuffer = m_queue->createCommandBuffer() };

        frame.commandbuffer.begin();
        frame.commandbuffer.beginRenderPass(*m_render_pass, frame.framebuffer);
        frame.commandbuffer.bindGraphicsPipeline(*m_pipeline);
        frame.commandbuffer.setViewport(0, viewports);
        frame.commandbuffer.setScissor(0, scissors);

        frame.commandbuffer.draw(3);

        frame.commandbuffer.endRenderPass();
        frame.commandbuffer.end();

        m_frame_datas.emplace_back(std::move(frame));
        ilog("Frame {} CommandBuffer and Framebuffer successfully created", i);
    }
}
