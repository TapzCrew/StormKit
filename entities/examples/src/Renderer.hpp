#pragma once

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/gpu/core/CommandBuffer.mpp>
#include <stormkit/gpu/core/Fwd.hpp>

#include <stormkit/gpu/pipeline/Framebuffer.mpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/Image.mpp>
#include <stormkit/gpu/resource/ImageView.mpp>

class Renderer {
  public:
    Renderer(const stormkit::wsi::Window &window);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    auto operator=(const Renderer &) -> Renderer & = delete;

    Renderer(Renderer &&) noexcept;
    auto operator=(Renderer &&) noexcept -> Renderer &;

    auto renderFrame() -> void;

    auto updateBoard(const stormkit::image::Image &board) -> void;

  private:
    auto doInitBaseRenderObjects() -> void;
    auto doInitMeshRenderObjects() -> void;
    auto doInitPerFrameObjects() -> void;

    const stormkit::wsi::Window *m_window = nullptr;

    stormkit::gpu::InstanceOwnedPtr m_instance;
    stormkit::gpu::DeviceOwnedPtr m_device;
    stormkit::gpu::SurfaceOwnedPtr m_surface;
    stormkit::gpu::Fence *m_current_fence = nullptr;

    const stormkit::gpu::Queue *m_queue = nullptr;
    std::vector<stormkit::gpu::ImageView> m_surface_views;

    stormkit::gpu::DescriptorSetLayoutOwnedPtr m_descriptor_set_layout;
    stormkit::gpu::DescriptorPoolOwnedPtr m_descriptor_pool;

    stormkit::gpu::RenderPassOwnedPtr m_render_pass;

    struct Board {
        std::vector<stormkit::gpu::Image> images;
        std::vector<stormkit::gpu::ImageView> image_views;
        stormkit::gpu::SamplerOwnedPtr sampler;

        stormkit::core::UInt32 current_image = 0;

        stormkit::gpu::ShaderOwnedPtr vertex_shader;
        stormkit::gpu::ShaderOwnedPtr fragment_shader;

        stormkit::gpu::GraphicsPipelineOwnedPtr pipeline;

        stormkit::gpu::DescriptorSetOwnedPtr descriptor_set;
    } m_board;

    std::vector<stormkit::gpu::CommandBuffer> m_command_buffers;
    std::vector<stormkit::gpu::Framebuffer> m_framebuffers;
};
