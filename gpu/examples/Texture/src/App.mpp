#pragma once

#include <stormkit/core/App.mpp>

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/gpu/core/CommandBuffer.mpp>
#include <stormkit/gpu/core/Fwd.hpp>

#include <stormkit/gpu/pipeline/Framebuffer.mpp>
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

    stormkit::gpu::DescriptorSetLayoutOwnedPtr m_descriptor_set_layout;
    stormkit::gpu::DescriptorPoolOwnedPtr m_descriptor_pool;

    stormkit::gpu::RenderPassOwnedPtr m_render_pass;
    stormkit::gpu::GraphicsPipelineOwnedPtr m_pipeline;

    stormkit::gpu::BufferOwnedPtr m_vertex_buffer;

    stormkit::gpu::DescriptorSetOwnedPtr m_descriptor_set;
    stormkit::gpu::ImageOwnedPtr m_texture;
    stormkit::gpu::ImageViewOwnedPtr m_texture_view;
    stormkit::gpu::SamplerOwnedPtr m_sampler;

    struct Frame {
        stormkit::gpu::Framebuffer framebuffer;
        stormkit::gpu::CommandBuffer commandbuffer;
    };

    std::vector<Frame> m_frame_datas;
};
