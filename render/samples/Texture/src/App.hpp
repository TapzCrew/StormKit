#pragma once

#include <storm/core/App.hpp>

#include <storm/window/Fwd.hpp>

#include <storm/image/Fwd.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

struct Camera {
    storm::core::Matrixf projection;
    storm::core::Matrixf view;
};

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

    Camera m_camera;
    storm::core::Matrixf m_model;

    storm::render::ShaderOwnedPtr m_vertex_shader;
    storm::render::ShaderOwnedPtr m_fragment_shader;

    storm::render::DescriptorSetLayoutOwnedPtr m_per_frame_descriptor_set_layout;
    storm::render::DescriptorSetLayoutOwnedPtr m_per_mesh_descriptor_set_layout;
    storm::render::DescriptorPoolOwnedPtr m_per_frame_descriptor_pool;
    storm::render::DescriptorPoolOwnedPtr m_per_mesh_descriptor_pool;

    storm::render::RenderPassOwnedPtr m_render_pass;
    storm::render::GraphicsPipelineOwnedPtr m_pipeline;

    storm::render::HardwareBufferOwnedPtr m_vertex_buffer;

    storm::render::DescriptorSetOwnedPtr m_camera_set;
    storm::render::HardwareBufferOwnedPtr m_camera_buffer;

    storm::render::DescriptorSetOwnedPtr m_mesh_data_set;
    storm::render::HardwareBufferOwnedPtr m_model_buffer;
    storm::image::ImageOwnedPtr m_image;
    storm::render::TextureOwnedPtr m_texture;
    storm::render::TextureViewOwnedPtr m_texture_view;
    storm::render::SamplerOwnedPtr m_sampler;

    struct Frame {
        storm::render::Framebuffer framebuffer;
        storm::render::CommandBuffer commandbuffer;
    };

    std::vector<Frame> m_frame_datas;
};
