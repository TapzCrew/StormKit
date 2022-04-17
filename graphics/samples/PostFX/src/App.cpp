#include "App.hpp"

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Vertex.hpp>
#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/mesh/Primitives.hpp>
#include <storm/engine/mesh/StaticMesh.hpp>
#include <storm/engine/mesh/StaticMeshComponent.hpp>
#include <storm/engine/mesh/StaticMeshRenderSystem.hpp>
#include <storm/engine/mesh/StaticSubMesh.hpp>

#include <storm/engine/material/MaterialComponent.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

using namespace storm;
using storm::log::operator""_module;

static constexpr auto WINDOW_WIDTH  = 1280u;
static constexpr auto WINDOW_HEIGHT = 768u;
static constexpr auto WINDOW_TITLE  = "StormKit PostFX Example";
static constexpr auto LOG_MODULE    = "PostFX"_module;

static const auto VERTEX_SHADER_DATA = std::vector<storm::core::UInt32> {
#include "mesh.vert.spv.hpp"
};
static const auto FRAGMENT_SHADER_DATA = std::vector<storm::core::UInt32> {
#include "mesh.frag.spv.hpp"
};

static const auto SEPIA_VERTEX_SHADER_DATA = std::vector<storm::core::UInt32> {
#include "sepia.vert.spv.hpp"
};
static const auto SEPIA_FRAGMENT_SHADER_DATA = std::vector<storm::core::UInt32> {
#include "sepia.frag.spv.hpp"
};

App::App() {
    log::LogHandler::ilog(LOG_MODULE,
                          "Using StormKit {}.{}.{} {} {}",
                          STORMKIT_MAJOR_VERSION,
                          STORMKIT_MINOR_VERSION,
                          STORMKIT_PATCH_VERSION,
                          STORMKIT_GIT_BRANCH,
                          STORMKIT_GIT_COMMIT_HASH);
    doInitWindow();
    doInitBaseRenderObjects();
    doInitMeshObjects();
}

App::~App() = default;

void App::run([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    while (m_window->isOpen()) {
        auto event = window::Event {};
        while (m_window->pollEvent(event))
            if (event.type == window::EventType::Closed) m_window->close();

        m_engine->render();
    }
}

void App::doInitWindow() {
    const auto video_settings =
        window::VideoSettings { .size = core::Extentu { .width  = WINDOW_WIDTH,
                                                        .height = WINDOW_HEIGHT } };
    const auto window_style = window::WindowStyle::Close;

    m_window = std::make_unique<window::Window>(WINDOW_TITLE, video_settings, window_style);
}

void App::doInitBaseRenderObjects() {
    m_engine = std::make_unique<engine::Engine>(*m_window);
    m_scene  = std::make_unique<engine::Scene>(*m_engine);

    m_mesh_render_system = core::makeObserver(
        m_scene->entities().addSystem<engine::StaticMeshRenderSystem>(*m_engine));

    m_engine->setInitFramegraphCallback(
        [this](auto &backbuffer, auto &framegraph) { doInitFramePasses(backbuffer, framegraph); });

    m_engine->setScene(*m_scene);
}

void App::doInitMeshObjects() {
    const auto &device = m_engine->device();

    const auto surface_extent  = m_engine->surface().extent();
    const auto surface_extentf = surface_extent.convertTo<storm::core::Extentf>();

    m_vertex_shader = device.createShaderPtr(VERTEX_SHADER_DATA, render::ShaderStage::Vertex);
    log::LogHandler::ilog(LOG_MODULE, "vertex shader loaded");
    m_fragment_shader = device.createShaderPtr(FRAGMENT_SHADER_DATA, render::ShaderStage::Fragment);
    log::LogHandler::ilog(LOG_MODULE, "fragment shader loaded");

    m_postfx_vertex_shader =
        device.createShaderPtr(SEPIA_VERTEX_SHADER_DATA, render::ShaderStage::Vertex);
    log::LogHandler::ilog(LOG_MODULE, "Sepia vertex shader loaded");
    m_postfx_fragment_shader =
        device.createShaderPtr(SEPIA_FRAGMENT_SHADER_DATA, render::ShaderStage::Fragment);
    log::LogHandler::ilog(LOG_MODULE, "Sepia fragment shader loaded");

    auto &world = m_scene->entities();
    auto mesh   = world.makeEntity();

    auto &mesh_component = world.addComponent<engine::StaticMeshComponent>(mesh);
    mesh_component.mesh =
        std::make_unique<engine::StaticMesh>(engine::makeTriangle(*m_engine, { 0.5f, 0.5f }));
    mesh_component.passes_to_render_in = { "ColorPass" };

    auto pipeline = render::GraphicsPipelineState {
        .color_blend_state = { .attachments = { {} } },
        .shader_state = { .shaders = core::makeConstObservers(m_vertex_shader, m_fragment_shader) },
        .viewport_state     = { .viewports = { { .position = { 0.f, 0.f },
                                             .extent   = surface_extentf,
                                             .depth    = { 0.f, 1.f } } },
                            .scissors  = { { .Int32 = { 0, 0 }, .extent = surface_extent } } },
        .vertex_input_state = { .binding_descriptions = m_vertices->bindingDescriptions(),
                                .input_attribute_descriptions =
                                    m_vertices->inputAttributeDescriptions() }
    };

    auto material = m_engine->createMaterial();
    material.setGraphicsPipelineState(std::move(pipeline));

    auto &material_component = world.addComponent<engine::MaterialComponent>(mesh);
    material_component.materials.emplace_back(std::move(material));

    m_postfx_state = render::GraphicsPipelineState {
        .color_blend_state = { .attachments = { {} } },
        .shader_state      = { .shaders = core::makeConstObservers(m_postfx_vertex_shader,
                                                              m_postfx_fragment_shader) },
        .viewport_state    = { .viewports = { { .position = { 0.f, 0.f },
                                             .extent   = surface_extentf,
                                             .depth    = { 0.f, 1.f } } },
                            .scissors  = { { .Int32 = { 0, 0 }, .extent = surface_extent } } },
    };
}

void App::doInitFramePasses(storm::engine::FramePassTextureID &backbuffer,
                            engine::FrameGraph &frame_graph) {
    const auto &surface = m_engine->surface();

    struct ColorPassData {
        engine::FramePassTextureID output;
    };

    const auto color_output_description = storm::engine::FramePassTextureDescription {
        .extent             = surface.extent(),
        .format             = surface.pixelFormat(),
        .source_layout      = render::TextureLayout::Undefined,
        .destination_layout = render::TextureLayout::Color_Attachment_Optimal
    };

    auto &color_pass = frame_graph.addPass<ColorPassData>(
        "ColorPass",
        [&color_output_description](auto &builder, auto &pass_data) {
            pass_data.output = builder.create("ColorPass:Output", color_output_description);
        },
        [this](const auto &pass_data, const auto &resources, auto &cmb) {
            m_mesh_render_system->render(cmb,
                                         "ColorPass",
                                         resources.renderpass(),
                                         { m_scene->cameraDescriptorSet() },
                                         { m_scene->cameraInt32() });
        });

    struct PostFXPassData {
        engine::FramePassTextureResourcePtr input;
        engine::FramePassTextureResourcePtr output;
    };

    auto &post_fx_pass = frame_graph.addPass<PostFXPassData>(
        "PostFXPass",
        [&color_pass, &backbuffer](auto &builder, auto &pass_data) {
            pass_data.input  = builder.read(*color_pass.data().output);
            pass_data.output = builder.write(backbuffer);
        },
        [this](const auto &pass_data, const auto &resources, auto &cmb) {
            m_postfx_state.layout.descriptor_set_layouts =
                storm::core::makeConstObservers(resources.descriptorSetLayout());

            auto &pipeline =
                m_engine->pipelineCache().getPipeline(m_postfx_state, resources.renderpass());

            cmb.bindGraphicsPipeline(pipeline);
            cmb.bindDescriptorSets(pipeline, { resources.descriptorSet() });
            cmb.draw(6);
        });

    post_fx_pass.setCullImune(true);
}
