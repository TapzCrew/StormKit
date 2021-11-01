#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/App.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

class App: public storm::core::App {
  public:
    App();
    ~App() override;

    void run(int argc, char **argv) override;

  private:
    void doInitWindow();
    void doInitBaseRenderObjects();
    void doInitMeshObjects();
    void doInitFramePasses(storm::engine::FramePassTextureResource &backbuffer,
                           storm::engine::FrameGraph &frame_graph);

    storm::window::WindowOwnedPtr m_window;
    storm::engine::EngineOwnedPtr m_engine;

    storm::engine::SceneOwnedPtr m_scene;
    storm::engine::StaticMeshRenderSystemPtr m_mesh_render_system;

    storm::render::ShaderOwnedPtr m_vertex_shader;
    storm::render::ShaderOwnedPtr m_fragment_shader;
    storm::render::ShaderOwnedPtr m_postfx_vertex_shader;
    storm::render::ShaderOwnedPtr m_postfx_fragment_shader;

    storm::engine::VertexArrayOwnedPtr m_vertices;

    storm::render::GraphicsPipelineState m_postfx_state;
};
