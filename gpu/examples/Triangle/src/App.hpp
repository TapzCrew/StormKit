#pragma once

#include <stormkit/core/App.hpp>

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/gpu/core/CommandBuffer.hpp>
#include <stormkit/gpu/core/Fwd.hpp>

#include <stormkit/gpu/pipeline/Framebuffer.hpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>

class App: public stormkit::core::App {
  public:
    App();
    ~App() override;

    auto run(const int argc, const char **argv) -> stormkit::core::Int32 override;

  private:
    auto doInitWindow() -> void;
    auto doInitBaseRenderObjects() -> void;
    auto doInitMeshRenderObjects() -> void;
    auto doInitPerFrameObjects() -> void;

    stormkit::wsi::WindowOwnedPtr m_window;
    bool m_fullscreen_enabled = false;

    stormkit::gpu::InstanceOwnedPtr m_instance;
    stormkit::gpu::DeviceOwnedPtr m_device;
    stormkit::gpu::SurfaceOwnedPtr m_surface;
    const stormkit::gpu::Queue *m_queue = nullptr;
    std::vector<stormkit::gpu::ImageView> m_surface_views;

    stormkit::gpu::ShaderOwnedPtr m_vertex_shader;
    stormkit::gpu::ShaderOwnedPtr m_fragment_shader;

    stormkit::gpu::RenderPassOwnedPtr m_render_pass;
    stormkit::gpu::GraphicsPipelineOwnedPtr m_pipeline;

    struct Frame {
        stormkit::gpu::Framebuffer framebuffer;
        stormkit::gpu::CommandBuffer commandbuffer;
    };

    std::vector<Frame> m_frame_datas;
};
