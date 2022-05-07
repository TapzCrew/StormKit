#pragma once

#include <storm/core/App.hpp>

#include <storm/window/Fwd.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

class App: public storm::core::App {
  public:
    App();
    ~App() override;

    void run(const int argc, const char **argv) override;

  private:
    void doInitWindow();
    void doInitBaseRenderObjects();
    void doInitMeshRenderObjects();

    storm::window::WindowOwnedPtr m_window;

    storm::render::InstanceOwnedPtr m_instance;
    storm::render::DeviceOwnedPtr m_device;
    storm::render::WindowSurfaceOwnedPtr m_surface;
    storm::render::QueueConstPtr m_queue;
    storm::render::TextureViewArray m_surface_views;

    storm::render::ShaderOwnedPtr m_vertex_shader;
    storm::render::ShaderOwnedPtr m_fragment_shader;

    storm::render::RenderPassOwnedPtr m_render_pass;
    storm::render::GraphicsPipelineOwnedPtr m_pipeline;

    storm::render::HardwareBufferOwnedPtr m_vertex_buffer;

    struct Frame {
        storm::render::Framebuffer framebuffer;
        storm::render::CommandBuffer commandbuffer;
    };

    std::vector<Frame> m_frame_datas;
};
