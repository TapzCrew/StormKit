/////////// - StormKit::core - ///////////
#include <storm/core/Strings.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/Fwd.hpp>
#include <storm/engine/NameComponent.hpp>
#include <storm/engine/Profiler.hpp>

#include <storm/engine/render/DrawableComponent.hpp>
#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/TransformComponent.hpp>

#include <storm/engine/render/3D/PbrMesh.hpp>
#include <storm/engine/render/3D/PbrRenderSystem.hpp>

#include <storm/engine/render/framegraph/FrameGraph.hpp>
#include <storm/engine/render/framegraph/FrameGraphNode.hpp>
#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

using namespace storm;
using namespace storm::engine;

static constexpr auto VERTEX_BINDING_DESCRIPTIONS = std::array {
    render::VertexBindingDescription { .binding = 0, .stride = sizeof(PbrMesh::Vertex) },
    render::VertexBindingDescription { .binding    = 1,
                                       .stride     = sizeof(RenderQueue::DrawData),
                                       .input_rate = render::VertexInputRate::Instance }
};
static constexpr auto VERTEX_ATTRIBUTE_DESCRIPTIONS = std::array {
    render::VertexInputAttributeDescription { .location = 0,
                                              .binding  = 0,
                                              .format   = render::Format::Float3,
                                              .offset   = offsetof(PbrMesh::Vertex, position) },
    render::VertexInputAttributeDescription { .location = 1,
                                              .binding  = 0,
                                              .format   = render::Format::Float3,
                                              .offset   = offsetof(PbrMesh::Vertex, normal) },
    render::VertexInputAttributeDescription { .location = 2,
                                              .binding  = 0,
                                              .format   = render::Format::Float2,
                                              .offset   = offsetof(PbrMesh::Vertex, texcoord) },
    render::VertexInputAttributeDescription { .location = 3,
                                              .binding  = 0,
                                              .format   = render::Format::Float4,
                                              .offset   = offsetof(PbrMesh::Vertex, tangent) },
    render::VertexInputAttributeDescription { .location = 4,
                                              .binding  = 0,
                                              .format   = render::Format::UInt4,
                                              .offset   = offsetof(PbrMesh::Vertex, joint_id) },
    render::VertexInputAttributeDescription { .location = 5,
                                              .binding  = 0,
                                              .format   = render::Format::Float4,
                                              .offset   = offsetof(PbrMesh::Vertex, weight) },
    render::VertexInputAttributeDescription { .location = 6,
                                              .binding  = 1,
                                              .format   = render::Format::Int,
                                              .offset =
                                                  offsetof(RenderQueue::DrawData, transform_id) },
    render::VertexInputAttributeDescription { .location = 7,
                                              .binding  = 1,
                                              .format   = render::Format::Int,
                                              .offset =
                                                  offsetof(RenderQueue::DrawData, material_id) },
    render::VertexInputAttributeDescription { .location = 8,
                                              .binding  = 1,
                                              .format   = render::Format::Int,
                                              .offset =
                                                  offsetof(RenderQueue::DrawData, skinning_id) }
};

static constexpr auto PBR_FORWARD_PER_MESH_DESCRIPTOR_SET_LAYOUT = std::array {
    render::DescriptorSetLayoutBinding { .binding = 0,
                                         .type    = render::DescriptorType::Storage_Buffer,
                                         .stages  = render::ShaderStage::Vertex |
                                                   render::ShaderStage::Fragment,
                                         .descriptor_count = 1 },
};

static constexpr auto PBR_FORWARD_PER_SCENE_DESCRIPTOR_SET_LAYOUT = std::array {
    render::DescriptorSetLayoutBinding { .binding = 0,
                                         .type    = render::DescriptorType::Uniform_Buffer,
                                         .stages  = render::ShaderStage::Vertex |
                                                   render::ShaderStage::Fragment,
                                         .descriptor_count = 1 },
};

static const auto PBR_FORWARD_VERTEX_SHADER_DATA = std::vector<core::UInt32> {
#include "PbrForward.vert.spv.hpp"
};
static const auto PBR_FORWARD_FRAGMENT_SHADER_DATA = std::vector<core::UInt32> {
#include "PbrForward.frag.spv.hpp"
};

/////////////////////////////////////
/////////////////////////////////////
PbrRenderSystem::PbrRenderSystem(Engine &engine, entities::EntityManager &manager)
    : RenderSystem { engine,
                     manager,
                     {
                         DrawableComponent::TYPE,
                     } },
      m_buffering_count { engine.surface().bufferingCount() },
      m_system_name { "StormKit:PbrRenderSystem" }, m_render_queue { engine,
                                                                     manager,
                                                                     m_buffering_count,
                                                                     m_system_name },
      m_default_camera { Camera::Type::Perspective, engine.surface().extent() },
      m_scene_global_buffer_pool { m_buffering_count } {
    m_last_update = std::chrono::high_resolution_clock::now();

    auto &resource_cache        = engine.resourceCache();
    m_pbr_forward_vertex_shader = core::makeObserver(
        resource_cache.requestShader(render::ShaderStage::Vertex, PBR_FORWARD_VERTEX_SHADER_DATA));
    m_pbr_forward_fragment_shader =
        core::makeObserver(resource_cache.requestShader(render::ShaderStage::Fragment,
                                                        PBR_FORWARD_FRAGMENT_SHADER_DATA));

    m_forward_plus_pass_name = core::format("{}:Forward+", m_system_name);
    m_update_scene_global_buffer_pass_name =
        core::format("{}:UpdateSceneGlobalBufferPass", m_system_name);

    setCamera(m_default_camera);
}

/////////////////////////////////////
/////////////////////////////////////
PbrRenderSystem::~PbrRenderSystem() = default;

/////////////////////////////////////
/////////////////////////////////////
PbrRenderSystem::PbrRenderSystem(PbrRenderSystem &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto PbrRenderSystem::operator=(PbrRenderSystem &&) noexcept -> PbrRenderSystem & = default;

/////////////////////////////////////
/////////////////////////////////////
void PbrRenderSystem::preUpdate() {
}

/////////////////////////////////////
/////////////////////////////////////
void PbrRenderSystem::update(core::Secondf delta) {
    m_cumuled_deltas += delta;

    auto now = std::chrono::high_resolution_clock::now();
    if (!m_first_iteration &&
        std::chrono::duration_cast<core::Secondf>(now - m_last_update).count() < UPDATE_FREQUENCY)
        return;

    for (auto e : m_to_be_added) {
        auto &drawable_component = m_manager->getComponent<DrawableComponent>(e);
        if (drawable_component.drawable->dirty())
            drawable_component.drawable->recomputeBoundingBox();

        m_render_queue.addDrawInstance(e, *drawable_component.drawable);

        m_entities.emplace_back(e);
    }

    m_to_be_added.clear();

    m_last_update = now;

    m_cumuled_deltas = core::Secondf { 0.f };
}

/////////////////////////////////////
/////////////////////////////////////
void PbrRenderSystem::setupFrameGraph(FrameGraph &frame_graph, FrameGraphResourceID backbuffer_id) {
    auto &engine        = this->engine();
    auto &cache         = engine.resourceCache();
    const auto &surface = engine.surface();
    const auto extent   = surface.extent();

    updateSceneGlobalBuffer(frame_graph);
    m_render_queue.update(frame_graph);

    const auto vertex_buffer_id       = m_render_queue.vertexBufferID();
    const auto index_buffer_id        = m_render_queue.indexBufferID();
    const auto draw_data_buffer_id    = m_render_queue.drawDataBufferID();
    const auto draw_command_buffer_id = m_render_queue.drawCommandBufferID();
    const auto transform_buffer_id    = m_render_queue.transformBufferID();

    struct ForwardPassData {
        FrameGraphResourceID output;
        FrameGraphResourceID depth;

        FrameGraphResourceID scene_global_buffer_id;
        FrameGraphResourceID vertex_buffer_id;
        FrameGraphResourceID index_buffer_id;
        FrameGraphResourceID draw_data_buffer_id;
        FrameGraphResourceID draw_command_buffer_id;
        FrameGraphResourceID transform_buffer_id;
    };

    auto &pass = frame_graph.addPassNode<ForwardPassData>(
        "StormKit:PbrRenderSystem:Forward+",
        [&backbuffer_id,
         &extent,
         this,
         &vertex_buffer_id,
         &index_buffer_id,
         &draw_data_buffer_id,
         &draw_command_buffer_id,
         &transform_buffer_id](ForwardPassData &pass_data, FrameGraphBuilder &builder) {
            const auto depth_descriptor =
                TextureDescriptor { .type    = render::TextureType::T2D,
                                    .format  = render::PixelFormat::Depth32F_Stencil8,
                                    .extent  = extent,
                                    .samples = render::SampleCountFlag::C1_BIT,
                                    .usage   = render::TextureUsage::Depth_Stencil_Attachment };

            pass_data.output = builder.write(backbuffer_id);
            pass_data.depth =
                builder.create("StormKit:PbrRenderSystem:Forward+:depth", depth_descriptor);

            pass_data.scene_global_buffer_id = builder.read(m_scene_global_buffer_id);
            pass_data.vertex_buffer_id       = builder.read(vertex_buffer_id);
            pass_data.index_buffer_id        = builder.read(index_buffer_id);
            pass_data.draw_data_buffer_id    = builder.read(draw_data_buffer_id);
            pass_data.draw_command_buffer_id = builder.read(draw_command_buffer_id);
            pass_data.transform_buffer_id    = builder.read(transform_buffer_id);
        },
        [&frame_graph, &cache, this](const FrameGraphStepData &step_data,
                                     const ForwardPassData &pass_data,
                                     render::CommandBuffer &cmb) {
            const auto &scene_global_buffer =
                frame_graph.getPhysicalBuffer(pass_data.scene_global_buffer_id);
            const auto &vertex_buffer = frame_graph.getPhysicalBuffer(pass_data.vertex_buffer_id);
            const auto &index_buffer  = frame_graph.getPhysicalBuffer(pass_data.index_buffer_id);
            const auto &draw_data_buffer =
                frame_graph.getPhysicalBuffer(pass_data.draw_data_buffer_id);
            const auto &draw_command_buffer =
                frame_graph.getPhysicalBuffer(pass_data.draw_command_buffer_id);
            const auto &transform_buffer =
                frame_graph.getPhysicalBuffer(pass_data.transform_buffer_id);

            const auto per_scene_descriptors =
                std::array { render::Descriptor { render::BufferDescriptor {
                    .type    = render::DescriptorType::Uniform_Buffer,
                    .binding = 0,
                    .buffer  = core::makeConstObserver(scene_global_buffer),
                    .range   = gsl::narrow_cast<core::UInt32>(scene_global_buffer.size()),
                    .offset  = 0u } } };
            const auto per_mesh_descriptors =
                std::array { render::Descriptor { render::BufferDescriptor {
                    .type    = render::DescriptorType::Storage_Buffer,
                    .binding = 0,
                    .buffer  = core::makeConstObserver(transform_buffer),
                    .range   = gsl::narrow_cast<core::UInt32>(transform_buffer.size()),
                    .offset  = 0u } } };

            const auto &per_scene_descriptor_set_layout =
                cache.requestDescriptorSetLayout(PBR_FORWARD_PER_SCENE_DESCRIPTOR_SET_LAYOUT);
            const auto &per_mesh_descriptor_set_layout =
                cache.requestDescriptorSetLayout(PBR_FORWARD_PER_MESH_DESCRIPTOR_SET_LAYOUT);

            auto &per_scene_descriptor_set =
                cache.requestDescriptorSet(per_scene_descriptor_set_layout,
                                           per_scene_descriptors,
                                           true);
            auto &per_mesh_descriptor_set =
                cache.requestDescriptorSet(per_mesh_descriptor_set_layout,
                                           per_mesh_descriptors,
                                           true);

            cmb.bindVertexBuffers(std::vector { std::cref(vertex_buffer),
                                                std::cref(draw_data_buffer) },
                                  std::vector { 0, 0 });
            cmb.bindIndexBuffer(index_buffer, 0, true);
            cmb.bindDescriptorSets(*step_data.pipeline,
                                   core::makeConstRefArray(per_scene_descriptor_set,
                                                           per_mesh_descriptor_set),
                                   {});

            // for (const auto &instance : m_instance_descriptors)
            cmb.drawIndexedIndirect(draw_command_buffer,
                                    0,
                                    m_render_queue.drawCommandCount(),
                                    sizeof(vk::DrawIndexedIndirectCommand));
        });

    auto &per_scene_descriptor_set_layout =
        cache.requestDescriptorSetLayout(PBR_FORWARD_PER_SCENE_DESCRIPTOR_SET_LAYOUT);
    auto &per_mesh_descriptor_set_layout =
        cache.requestDescriptorSetLayout(PBR_FORWARD_PER_MESH_DESCRIPTOR_SET_LAYOUT);

    const auto state = render::GraphicsPipelineState {
        .viewport_state      = { .viewports = { render::Viewport { .position = { 0.f, 0.f },
                                                              .extent   = extent,
                                                              .depth    = { 0.f, 1.f } } },
                            .scissors  = { render::Scissor { .offset = { 0, 0 },
                                                            .extent = extent } } },
        .rasterization_state = { .cull_mode = render::CullMode::Front },
        .color_blend_state   = { .attachments = { {} } },
        .shader_state       = { .shaders = core::makeConstObserverArray(m_pbr_forward_vertex_shader,
                                                                  m_pbr_forward_fragment_shader) },
        .vertex_input_state = { .binding_descriptions         = VERTEX_BINDING_DESCRIPTIONS,
                                .input_attribute_descriptions = VERTEX_ATTRIBUTE_DESCRIPTIONS },
        .depth_stencil_state = { .depth_test_enable = true, .depth_write_enable = true },
        .layout              = { .descriptor_set_layouts = { core::makeConstObserver(
                                                    per_scene_descriptor_set_layout),
                                                core::makeConstObserver(
                                                    per_mesh_descriptor_set_layout) } }
    };

    pass.setExtent(extent);
    pass.setGraphicsPipeline(state);

    m_frame_counter += 1;
    m_frame_counter %= m_buffering_count;

    m_first_iteration = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto PbrRenderSystem::updateSceneGlobalBuffer(FrameGraph &frame_graph) -> void {
    struct UpdateSceneGlobalPass {
        FrameGraphResourceID staging_buffer_id;
        FrameGraphResourceID scene_global_buffer_id;
    };

    const auto reupload = m_camera->dirty() || m_camera_switched;

    const auto scene_global_buffer_name =
        core::format("{}:SceneGlobalBuffer:{}", m_system_name, m_frame_counter);

    if (!reupload) {
        auto &scene_global_buffer = m_scene_global_buffer_pool.get();

        const auto scene_global_buffer_descriptor =
            BufferDescriptor { .size              = scene_global_buffer.size(),
                               .usage             = scene_global_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_scene_global_buffer_id = frame_graph.setRetainedResource(scene_global_buffer_name,
                                                                   scene_global_buffer_descriptor,
                                                                   scene_global_buffer);
        return;
    }

    auto &engine       = this->engine();
    const auto &device = engine.device();

    constexpr auto staging_buffer_size = sizeof(Camera::Data);

    if (!m_scene_global_buffer_pool.empty()) m_scene_global_buffer_pool.next();
    m_scene_global_buffer_pool.emplace(device,
                                       render::HardwareBufferUsage::Uniform |
                                           render::HardwareBufferUsage::Transfert_Dst,
                                       staging_buffer_size,
                                       render::MemoryProperty::Device_Local);

    auto &scene_global_buffer = m_scene_global_buffer_pool.get();
    device.setObjectName(scene_global_buffer, scene_global_buffer_name);

    const auto scene_global_buffer_descriptor =
        BufferDescriptor { .size              = scene_global_buffer.size(),
                           .usage             = scene_global_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_scene_global_buffer_id = frame_graph.setRetainedResource(scene_global_buffer_name,
                                                               scene_global_buffer_descriptor,
                                                               scene_global_buffer);

    auto &update_scene_global_pass = frame_graph.addPassNode<UpdateSceneGlobalPass>(
        m_update_scene_global_buffer_pass_name,
        [this](auto &pass_data, FrameGraphBuilder &builder) {
            const auto staging_buffer_descriptor =
                BufferDescriptor { .size              = staging_buffer_size,
                                   .usage             = render::HardwareBufferUsage::Transfert_Src,
                                   .memory_properties = render::MemoryProperty::Host_Visible |
                                                        render::MemoryProperty::Host_Coherent };

            pass_data.staging_buffer_id =
                builder.create("StormKit:PbrRenderSystem:SceneGlobalsPass:StagingBuffer",
                               staging_buffer_descriptor);
            pass_data.scene_global_buffer_id = builder.write(m_scene_global_buffer_id);
        },
        [this,
         &frame_graph]([[maybe_unused]] const auto &step_data, const auto &pass_data, auto &cmb) {
            auto &staging_buffer = frame_graph.getPhysicalBuffer(pass_data.staging_buffer_id);
            auto &scene_global_buffer =
                frame_graph.getPhysicalBuffer(pass_data.scene_global_buffer_id);

            staging_buffer.upload(core::toConstByteSpan(&m_camera->data()));
            staging_buffer.flush(0u, staging_buffer_size);

            cmb.copyBuffer(staging_buffer, scene_global_buffer, staging_buffer_size, 0u, 0u);
        },
        QueueFlag::Async_Transfert);
    update_scene_global_pass.setCullImmune(true);

    m_camera_switched = false;
}
