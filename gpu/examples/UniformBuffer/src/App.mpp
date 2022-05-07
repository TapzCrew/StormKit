#pragma once

#include <stormkit/core/App.mpp>

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/gpu/core/CommandBuffer.mpp>
#include <stormkit/gpu/core/Fwd.hpp>

#include <stormkit/gpu/pipeline/DescriptorSet.mpp>
#include <stormkit/gpu/pipeline/Framebuffer.mpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>

#include <stormkit/gpu/resource/Buffer.mpp>
#include <stormkit/gpu/resource/Fwd.hpp>

#include "Constants.mpp"

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

    auto updateModelMatrix(stormkit::core::Secondf &delta) -> void;

    stormkit::wsi::WindowOwnedPtr m_window;
    bool m_fullscreen_enabled = false;

    stormkit::gpu::InstanceOwnedPtr m_instance;
    stormkit::gpu::DeviceOwnedPtr m_device;
    stormkit::gpu::SurfaceOwnedPtr m_surface;
    const stormkit::gpu::Queue *m_queue = nullptr;
    std::vector<stormkit::gpu::ImageView> m_surface_views;

    Camera m_camera;
    Transform m_model;

    stormkit::gpu::ShaderOwnedPtr m_vertex_shader;
    stormkit::gpu::ShaderOwnedPtr m_fragment_shader;

    stormkit::gpu::DescriptorSetLayoutOwnedPtr m_descriptor_set_layout;
    stormkit::gpu::DescriptorPoolOwnedPtr m_descriptor_pool;

    stormkit::gpu::RenderPassOwnedPtr m_render_pass;
    stormkit::gpu::GraphicsPipelineOwnedPtr m_pipeline;

    stormkit::gpu::BufferOwnedPtr m_vertex_buffer;

    stormkit::gpu::DescriptorSetOwnedPtr m_descriptor_set;
    stormkit::gpu::BufferOwnedPtr m_camera_buffer;

    stormkit::gpu::DescriptorSetOwnedPtr m_model_descriptor_set;
    stormkit::gpu::BufferOwnedPtr m_model_staging_buffer;
    stormkit::gpu::BufferOwnedPtr m_model_buffer;

    struct Frame {
        stormkit::gpu::Framebuffer framebuffer;
        stormkit::gpu::CommandBuffer commandbuffer;
    };

    std::vector<stormkit::gpu::Image> m_depth_buffers;
    std::vector<stormkit::gpu::ImageView> m_depth_buffer_views;
    std::vector<Frame> m_frame_datas;
};
