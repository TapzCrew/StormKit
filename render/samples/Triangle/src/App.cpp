#include "App.hpp"

#include <storm/log/LogHandler.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/WindowSurface.hpp>

using namespace storm;
using log::operator""_module;

struct Vertex {
    core::Vector2f position;
    core::Vector3f color;
};

static constexpr auto WINDOW_TITLE = "StormKit Triangle Example";
static constexpr auto LOG_MODULE   = "Triangle"_module;
static constexpr auto MESH_VERTICES =
    std::array { Vertex { .position = { 0.0f, -0.5f }, .color = { 1.0f, 0.0f, 0.0f } },
                 Vertex { .position = { 0.5f, 0.5f }, .color = { 0.0f, 1.0f, 0.0f } },
                 Vertex { .position = { -0.5f, 0.5f }, .color = { 0.0f, 0.0f, 1.0f } } };
static constexpr auto MESH_VERTEX_BUFFER_SIZE = sizeof(Vertex) * std::size(MESH_VERTICES);
static constexpr auto MESH_VERTEX_BINDING_DESCRIPTIONS =
    std::array { render::VertexBindingDescription { .binding = 0, .stride = sizeof(Vertex) } };
static constexpr auto MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS =
    std::array { render::VertexInputAttributeDescription { .location = 0,
                                                           .binding  = 0,
                                                           .format   = render::Format::Float2,
                                                           .offset   = offsetof(Vertex, position) },
                 render::VertexInputAttributeDescription { .location = 1,
                                                           .binding  = 0,
                                                           .format   = render::Format::Float3,
                                                           .offset   = offsetof(Vertex, color) } };

static const auto VERTEX_SHADER_DATA = std::vector<core::UInt32> {
#include "vertex.vert.spv.hpp"
};
static const auto FRAGMENT_SHADER_DATA = std::vector<core::UInt32> {
#include "fragment.frag.spv.hpp"
};

App::App() {
    doInitWindow();
    doInitBaseRenderObjects();
    doInitMeshRenderObjects();
}

App::~App() {
    log::LogHandler::ilog(LOG_MODULE, "Cleaning");

    m_device->waitIdle();
}

void App::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) {
    while (m_window->isOpen()) {
        auto event = window::Event {};
        while (m_window->pollEvent(event)) {
            if (event.type == window::EventType::Closed) m_window->close();
            else if (event.type == window::EventType::KeyReleased &&
                     event.key_event.key == window::Key::Escape)
                m_window->close();
        }

        auto frame       = m_surface->acquireNextFrame();
        auto &frame_data = m_frame_datas[frame.texture_index % m_surface->bufferingCount()];

        frame_data.commandbuffer.submit(core::makeConstObserverArray(frame.texture_available),
                                        core::makeConstObserverArray(frame.render_finished),
                                        frame.in_flight);

        m_surface->present(frame);
    }
}

void App::doInitWindow() {
    // First we create the window
    const auto window_style = window::WindowStyle::Close;

    m_window = std::make_unique<window::Window>(WINDOW_TITLE,
                                                window::VideoSettings { .size = { 800u, 600u } },
                                                window_style);
}

void App::doInitBaseRenderObjects() {
    // We create an instance and initialize device on best available GPU
    m_instance = std::make_unique<render::Instance>();
    log::LogHandler::ilog(LOG_MODULE, "Render backend successfully initialized");
    log::LogHandler::ilog(LOG_MODULE,
                          "Using StormKit {}.{}.{} {} {}",
                          STORMKIT_MAJOR_VERSION,
                          STORMKIT_MINOR_VERSION,
                          STORMKIT_PATCH_VERSION,
                          STORMKIT_GIT_BRANCH,
                          STORMKIT_GIT_COMMIT_HASH);

    m_surface = m_instance->createWindowSurfacePtr(*m_window);

    const auto &physical_device      = m_instance->pickPhysicalDevice(*m_surface);
    const auto &physical_device_info = physical_device.info();

    log::LogHandler::ilog(LOG_MODULE, "Using physical device {}", physical_device_info.device_name);
    log::LogHandler::ilog(LOG_MODULE, "{}", physical_device_info);

    m_device = physical_device.createLogicalDevicePtr();
    log::LogHandler::ilog(LOG_MODULE, "Device successfully created");

    // We initialize the surface (it will acquire images and init all surface default parameters
    m_surface->initialize(*m_device);
    log::LogHandler::ilog(LOG_MODULE,
                          "Device successfully initialized with {} image(s)",
                          m_surface->textureCount());

    m_queue = core::makeConstObserver(m_device->graphicsQueue());

    for (const auto &texture : m_surface->textures())
        m_surface_views.emplace_back(texture.createView());
}

void App::doInitMeshRenderObjects() {
    const auto surface_extent  = core::Extentf { m_surface->extent() };
    const auto buffering_count = m_surface->bufferingCount();

    // We load our triangle shaders
    m_vertex_shader = m_device->createShaderPtr(VERTEX_SHADER_DATA, render::ShaderStage::Vertex);
    log::LogHandler::ilog(LOG_MODULE, "Vertex shader loaded");
    m_fragment_shader =
        m_device->createShaderPtr(FRAGMENT_SHADER_DATA, render::ShaderStage::Fragment);
    log::LogHandler::ilog(LOG_MODULE, "Fragment shader loaded");

    // We need to create a render pass, the render pass describe how the framebuffer will look
    auto description = render::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() } },
        .subpasses   = { { .bind_point      = render::PipelineBindPoint::Graphics,
                         .attachment_refs = { { .attachment_id = 0u } } } }
    };
    m_render_pass = m_device->createRenderPassPtr(std::move(description));
    log::LogHandler::ilog(LOG_MODULE, "Renderpass successfully created");

    // We create a pipeline, the pipeline describe all the fixed function parameters and the shaders
    // wich will be bound
    m_pipeline       = m_device->createGraphicsPipelinePtr();
    const auto state = render::GraphicsPipelineState {
        .viewport_state     = { .viewports = { render::Viewport { .position = { 0.f, 0.f },
                                                              .extent   = surface_extent,
                                                              .depth    = { 0.f, 1.f } } },
                            .scissors  = { render::Scissor { .offset = { 0, 0 },
                                                            .extent = surface_extent } } },
        .color_blend_state  = { .attachments = { {} } },
        .shader_state       = { .shaders =
                              core::makeConstObserverArray(m_vertex_shader, m_fragment_shader) },
        .vertex_input_state = { .binding_descriptions = MESH_VERTEX_BINDING_DESCRIPTIONS,
                                .input_attribute_descriptions =
                                    MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS },
    };

    m_pipeline->setState(std::move(state));
    m_pipeline->setRenderPass(*m_render_pass);
    m_pipeline->build();
    log::LogHandler::ilog(LOG_MODULE, "engine pipeline successfully created");

    // We create our mesh data, we only need a vertex buffer
    m_vertex_buffer = m_device->createVertexBufferPtr(MESH_VERTEX_BUFFER_SIZE);
    m_vertex_buffer->upload<Vertex>(MESH_VERTICES);
    m_vertex_buffer->flush(0, MESH_VERTEX_BUFFER_SIZE);
    log::LogHandler::ilog(LOG_MODULE,
                          "{} bytes uploaded to vertex buffer",
                          MESH_VERTEX_BUFFER_SIZE);

    // Finally we create per frame data, we need a commandbuffer and a framebuffer by frame to avoid
    // waiting the GPU
    m_frame_datas.reserve(buffering_count);
    for (auto i = 0u; i < buffering_count; ++i) {
        const auto &texture_view = m_surface_views[i];

        auto frame =
            Frame { .framebuffer   = m_render_pass->createFramebuffer(surface_extent,
                                                                    core::makeConstObserverArray(
                                                                        texture_view)),
                    .commandbuffer = m_queue->createCommandBuffer() };

        frame.commandbuffer.begin();
        frame.commandbuffer.beginRenderPass(*m_render_pass, frame.framebuffer);
        frame.commandbuffer.bindGraphicsPipeline(*m_pipeline);
        frame.commandbuffer.bindVertexBuffers({ *m_vertex_buffer }, { 0 });

        frame.commandbuffer.draw(gsl::narrow_cast<core::Int32>(std::size(MESH_VERTICES)));

        frame.commandbuffer.endRenderPass();
        frame.commandbuffer.end();

        frame.commandbuffer.build();

        m_frame_datas.emplace_back(std::move(frame));
        log::LogHandler::ilog(LOG_MODULE,
                              "Frame {} CommandBuffer and Framebuffer successfully created",
                              i);
    }
}
