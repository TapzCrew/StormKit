#include "App.hpp"

#include <storm/log/LogHandler.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/WindowSurface.hpp>

using namespace storm;
using log::operator""_module;

struct MeshVertex {
    core::Vector3f position;
    core::Vector3f color;
};

static constexpr auto WINDOW_TITLE  = "StormKit UniformBuffer Example";
static constexpr auto LOG_MODULE    = "UniformBuffer"_module;
static constexpr auto MESH_VERTICES = std::array {
    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { -1.f, -1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { -1.f, 1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },

    MeshVertex { .position = { 1.f, 1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { -1.f, 1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },

    MeshVertex { .position = { 1.f, -1.f, 1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { 1.f, -1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },

    MeshVertex { .position = { 1.f, 1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { 1.f, -1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },

    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, 1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, 1.f, -1.f }, .color = { 0.f, 0.f, 1.f } },

    MeshVertex { .position = { 1.f, -1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, -1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, -1.f, -1.f }, .color = { 0.f, 0.f, 1.f } },

    MeshVertex { .position = { -1.f, 1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { -1.f, -1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { 1.f, -1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },

    MeshVertex { .position = { 1.f, 1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { 1.f, -1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },
    MeshVertex { .position = { 1.f, 1.f, -1.f }, .color = { 1.f, 0.f, 0.f } },

    MeshVertex { .position = { 1.f, -1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { 1.f, 1.f, 1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { 1.f, -1.f, 1.f }, .color = { 0.f, 1.f, 0.f } },

    MeshVertex { .position = { 1.f, 1.f, 1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { 1.f, 1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },
    MeshVertex { .position = { -1.f, 1.f, -1.f }, .color = { 0.f, 1.f, 0.f } },

    MeshVertex { .position = { 1.f, 1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, 1.f, -1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, 1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },

    MeshVertex { .position = { 1.f, 1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { -1.f, 1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
    MeshVertex { .position = { 1.f, -1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
};

static constexpr auto MESH_VERTEX_BUFFER_SIZE = sizeof(MeshVertex) * std::size(MESH_VERTICES);
static constexpr auto MESH_VERTEX_BINDING_DESCRIPTIONS =
    std::array { render::VertexBindingDescription { .binding = 0, .stride = sizeof(MeshVertex) } };
static constexpr auto MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS = std::array {
    render::VertexInputAttributeDescription { .location = 0,
                                              .binding  = 0,
                                              .format   = render::Format::Float3,
                                              .offset   = offsetof(MeshVertex, position) },
    render::VertexInputAttributeDescription { .location = 1,
                                              .binding  = 0,
                                              .format   = render::Format::Float3,
                                              .offset   = offsetof(MeshVertex, color) }
};
static constexpr auto CAMERA_BUFFER_SIZE = sizeof(Camera);
static constexpr auto MODEL_BUFFER_SIZE  = sizeof(Transform);
static const auto VERTEX_SHADER_DATA     = std::vector<core::UInt32> {
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

    m_frame_datas.clear();
}

void App::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) {
    namespace Chrono = std::chrono;
    using Clock      = std::chrono::high_resolution_clock;

    auto start_time = Clock::now();
    while (m_window->isOpen()) {
        auto event = window::Event {};
        while (m_window->pollEvent(event)) {
            if (event.type == window::EventType::Closed) m_window->close();
            else if (event.type == window::EventType::KeyReleased &&
                     event.key_event.key == window::Key::Escape)
                m_window->close();
        }

        auto frame       = m_surface->acquireNextFrame();
        auto &frame_data = m_frame_datas[frame.texture_index];

        // we update our mesh model matrix
        const auto now_timepoint = Clock::now();
        const auto time =
            Chrono::duration<float, Chrono::seconds::period> { now_timepoint - start_time }.count();

        m_model.model = core::rotate(core::Matrix { 1.f },
                                     time * core::radians(90.f),
                                     core::Vector3f { 0.f, 0.f, 1.f });

        frame_data.staging_buffer.upload<Transform>({ &m_model, 1 });
        frame_data.staging_buffer.flush(0, MODEL_BUFFER_SIZE);

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
                                                window::VideoSettings { .size = { 800, 600 } },
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

    // We initialize the camera et set the model matrix to default
    m_camera = {
        .projection = core::perspective(core::radians(45.f),
                                        surface_extent.width / surface_extent.height,
                                        0.1f,
                                        10.f),
        .view = core::lookAt(core::Vector3f { 4.f, 3.f, 3.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f })
    };
    m_model.model = core::Matrix { 1.f };

    // We load our triangle shaders
    m_vertex_shader = m_device->createShaderPtr(VERTEX_SHADER_DATA, render::ShaderStage::Vertex);
    log::LogHandler::ilog(LOG_MODULE, "Vertex shader loaded");
    m_fragment_shader =
        m_device->createShaderPtr(FRAGMENT_SHADER_DATA, render::ShaderStage::Fragment);
    log::LogHandler::ilog(LOG_MODULE, "Fragment shader loaded");

    // We need to indicate to the pipeline how we pass data to the shader, first we need two
    // descriptor layout, on per frame and on per mesh
    m_per_frame_descriptor_set_layout = m_device->createDescriptorSetLayoutPtr();
    m_per_frame_descriptor_set_layout->addBinding({ .binding = 0,
                                                    .type = render::DescriptorType::Uniform_Buffer,
                                                    .stages           = render::ShaderStage::Vertex,
                                                    .descriptor_count = 1 });
    m_per_frame_descriptor_set_layout->bake();

    m_per_mesh_descriptor_set_layout = m_device->createDescriptorSetLayoutPtr();
    m_per_mesh_descriptor_set_layout->addBinding({ .binding = 0,
                                                   .type   = render::DescriptorType::Uniform_Buffer,
                                                   .stages = render::ShaderStage::Vertex,
                                                   .descriptor_count = 1 });
    m_per_mesh_descriptor_set_layout->bake();

    // Next we need two pool (per frame and per mesh) to allocate descriptors
    m_per_frame_descriptor_pool =
        m_device->createDescriptorPoolPtr({ { render::DescriptorType::Uniform_Buffer, 1 } }, 1);
    m_per_mesh_descriptor_pool =
        m_device->createDescriptorPoolPtr({ { render::DescriptorType::Uniform_Buffer,
                                              buffering_count } },
                                          buffering_count);

    // We need to create a render pass, the render pass describe how the framebuffer will look
    auto description = render::RenderPassDescription {
        .attachments = { { .format = m_surface->pixelFormat() },
                         { .format = render::PixelFormat::Depth32F_Stencil8,
                           .destination_layout =
                               render::TextureLayout::Depth_Stencil_Attachment_Optimal } },
        .subpasses   = { { .bind_point      = render::PipelineBindPoint::Graphics,
                         .attachment_refs = { { .attachment_id = 0u },
                                              { .attachment_id = 1u,
                                                .layout        = render::TextureLayout::
                                                    Depth_Stencil_Attachment_Optimal } } } }
    };
    m_render_pass = m_device->createRenderPassPtr(std::move(description));
    log::LogHandler::ilog(LOG_MODULE, "Renderpass successfully created");

    // We create a pipeline, the pipeline describe all the fixed function parameters and the shaders
    // wich will be bound
    m_pipeline       = m_device->createGraphicsPipelinePtr();
    const auto state = render::GraphicsPipelineState {
        .viewport_state      = { .viewports = { render::Viewport { .position = { 0.f, 0.f },
                                                              .extent   = surface_extent,
                                                              .depth    = { 0.f, 1.f } } },
                            .scissors  = { render::Scissor { .offset = { 0, 0 },
                                                            .extent = surface_extent } } },
        .color_blend_state   = { .attachments = { {} } },
        .shader_state        = { .shaders =
                              core::makeConstObserverArray(m_vertex_shader, m_fragment_shader) },
        .vertex_input_state  = { .binding_descriptions = MESH_VERTEX_BINDING_DESCRIPTIONS,
                                .input_attribute_descriptions =
                                    MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS },
        .depth_stencil_state = { .depth_test_enable  = true,
                                 .depth_write_enable = true,
                                 .depth_compare_op   = render::CompareOperation::Less },
        .layout              = { .descriptor_set_layouts =
                        core::makeConstObserverArray(m_per_frame_descriptor_set_layout,
                                                     m_per_mesh_descriptor_set_layout) }
    };

    m_pipeline->setState(std::move(state));
    m_pipeline->setRenderPass(*m_render_pass);
    m_pipeline->build();
    log::LogHandler::ilog(LOG_MODULE, "engine pipeline successfully created");

    // We create our mesh data, we only need a vertex buffer
    m_vertex_buffer = m_device->createVertexBufferPtr(MESH_VERTEX_BUFFER_SIZE);
    m_vertex_buffer->upload<MeshVertex>(MESH_VERTICES);
    log::LogHandler::ilog(LOG_MODULE,
                          "{} bytes uploaded to vertex buffer",
                          MESH_VERTEX_BUFFER_SIZE);

    // We allocate the set and prepare data
    m_camera_set =
        m_per_frame_descriptor_pool->allocateDescriptorSetPtr(*m_per_frame_descriptor_set_layout);

    m_camera_buffer = m_device->createUniformBufferPtr(CAMERA_BUFFER_SIZE);
    m_camera_buffer->upload<Camera>({ &m_camera, 1 });
    m_camera_buffer->flush(0, CAMERA_BUFFER_SIZE);
    log::LogHandler::ilog(LOG_MODULE, "{} bytes uploaded to camera buffer", CAMERA_BUFFER_SIZE);

    m_camera_set->update(std::array<render::Descriptor, 1> {
        render::BufferDescriptor { .binding = 0,
                                   .buffer  = core::makeConstObserver(m_camera_buffer),
                                   .range   = CAMERA_BUFFER_SIZE } });

    // Finally we create per frame data, we need a commandbuffer and a framebuffer by frame to avoid
    // waiting the GPU
    // and we prepare a buffer and a staging buffer by frame and it associed set
    m_depth_buffers.reserve(buffering_count);
    m_depth_buffer_views.reserve(buffering_count);
    m_frame_datas.reserve(buffering_count);
    for (auto i = 0u; i < buffering_count; ++i) {
        const auto &texture_view = m_surface_views[i];

        auto depth = m_device->createTexturePtr(surface_extent,
                                                render::PixelFormat::Depth32F_Stencil8,
                                                1u,
                                                1u,
                                                render::TextureType::T2D,
                                                render::TextureCreateFlag::None,
                                                render::SampleCountFlag::C1_BIT,
                                                render::TextureUsage::Depth_Stencil_Attachment);

        auto depth_view = depth->createViewPtr(render::TextureViewType::T2D,
                                               { .aspect_mask = render::TextureAspectMask::Depth });

        auto frame =
            Frame { .framebuffer =
                        m_render_pass->createFramebuffer(surface_extent,
                                                         { core::makeConstObserver(texture_view),
                                                           core::makeConstObserver(depth_view) }),
                    .commandbuffer  = m_queue->createCommandBuffer(),
                    .descriptor_set = m_per_mesh_descriptor_pool->allocateDescriptorSet(
                        *m_per_mesh_descriptor_set_layout),
                    .staging_buffer = m_device->createStagingBuffer(MODEL_BUFFER_SIZE),
                    .model_buffer =
                        m_device->createUniformBuffer(MODEL_BUFFER_SIZE,
                                                      render::MemoryProperty::Device_Local,
                                                      true) };

        frame.descriptor_set.update(render::DescriptorStaticArray<1> {
            render::BufferDescriptor { .binding = 0,
                                       .buffer  = core::makeConstObserver(frame.model_buffer),
                                       .range   = MODEL_BUFFER_SIZE } });

        frame.commandbuffer.begin();
        frame.commandbuffer.copyBuffer(frame.staging_buffer, frame.model_buffer, MODEL_BUFFER_SIZE);
        frame.commandbuffer.beginRenderPass(*m_render_pass,
                                            frame.framebuffer,
                                            { render::ClearColor {},
                                              render::ClearDepthStencil {} });
        frame.commandbuffer.bindGraphicsPipeline(*m_pipeline);

        frame.commandbuffer.bindDescriptorSets(*m_pipeline,
                                               { *m_camera_set, frame.descriptor_set });
        frame.commandbuffer.bindVertexBuffers({ *m_vertex_buffer }, { 0 });

        frame.commandbuffer.draw(gsl::narrow_cast<core::Int32>(std::size(MESH_VERTICES)));

        frame.commandbuffer.endRenderPass();
        frame.commandbuffer.end();

        frame.commandbuffer.build();

        m_depth_buffers.emplace_back(std::move(depth));
        m_depth_buffer_views.emplace_back(std::move(depth_view));
        m_frame_datas.emplace_back(std::move(frame));
        log::LogHandler::ilog(
            LOG_MODULE,
            "Frame {} CommandBuffer, Framebuffer, set and buffers successfully created",
            i);
    }
}
