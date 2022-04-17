/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/render/Drawable.hpp>
#include <storm/engine/render/DrawableComponent.hpp>
#include <storm/engine/render/RenderQueue.hpp>
#include <storm/engine/render/TransformComponent.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
RenderQueue::RenderQueue(Engine &engine,
                         entities::EntityManager &manager,
                         core::UInt32 buffering_count,
                         std::string_view system_name)
    : m_engine { engine }, m_manager { manager }, m_buffering_count { buffering_count },
      m_system_name { std::move(system_name) }, m_vertex_buffer_pool { m_buffering_count },
      m_index_buffer_pool { m_buffering_count }, m_draw_data_buffer_pool { m_buffering_count },
      m_draw_command_buffer_pool { m_buffering_count },
      m_transform_buffer_pool { m_buffering_count }, m_material_buffer_pool { m_buffering_count } {
    m_vertex_buffer_name       = core::format("{}:VertexBuffer", m_system_name);
    m_index_buffer_name        = core::format("{}:IndexBuffer", m_system_name);
    m_draw_data_buffer_name    = core::format("{}:DrawDataBuffer", m_system_name);
    m_draw_command_buffer_name = core::format("{}:DrawCommandBuffer", m_system_name);

    m_transform_buffer_name = core::format("{}:TransformBuffer", m_system_name);
    m_material_buffer_name  = core::format("{}:MaterialBuffer", m_system_name);

    m_update_vertices_and_indices_pass_name =
        core::format("{}:UpdateVerticesAndIndicesBufferPass", m_system_name);
    m_update_draw_data_buffer_pass_name = core::format("{}:UpdateDrawDataBufferPass", m_system_name);
    m_update_draw_command_buffer_pass_name =
        core::format("{}:UpdateDrawCommandBufferPass", m_system_name);
    m_update_transform_buffer_pass_name =
        core::format("{}:UpdateTransformBufferPass", m_system_name);

    m_transforms.emplace_back(
        Transform::Data { core::Matrix { 1.f }, core::inverse(core::Matrix { 1.f }) });
}

/////////////////////////////////////
/////////////////////////////////////
RenderQueue::~RenderQueue() = default;

/////////////////////////////////////
/////////////////////////////////////
RenderQueue::RenderQueue(RenderQueue &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::operator=(RenderQueue &&) noexcept -> RenderQueue & = default;

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::addDrawInstance(entities::Entity e, const Drawable &drawable) -> void {
    using DrawEntryPtr = DrawEntry *;

    const auto &manager = m_manager.get();

    auto hash = std::hash<Drawable> {}(drawable);

    const auto predicate = [&hash](const auto &entry) { return entry.hash == hash; };

    auto it    = core::ranges::find_if(m_draws, predicate);
    auto entry = DrawEntryPtr { nullptr };

    if (it == std::end(m_draws)) entry = &m_draws.emplace_back(DrawEntry { .hash = hash });
    else
        entry = &*it;

    auto transform_id = 0u;
    if (manager.hasComponent<TransformComponent>(e)) {
        transform_id = std::size(m_transforms);
        m_transforms.emplace_back(
            Transform::Data { core::Matrix { 1.f }, core::inverse(core::Matrix { 1.f }) });
    }

    const auto id = gsl::narrow_cast<core::UInt32>(std::size(m_draw_data));
    m_draw_data.emplace_back(DrawData { .transform_id = transform_id });

    entry->draw_instances.emplace_back(std::size(m_draw_instances));
    m_draw_instances.emplace_back(DrawInstance { id, e });
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::update(FrameGraph &frame_graph) -> void {
    updateVertexAndIndexBuffer(frame_graph);
    updateDrawDataBuffer(frame_graph);
    updateDrawCommandsBuffer(frame_graph);
    updateTransformBuffer(frame_graph);

    m_frame_counter += 1;
    m_frame_counter %= m_buffering_count;
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::updateVertexAndIndexBuffer(FrameGraph &frame_graph) -> void {
    struct UpdateVerticesAndIndicesPass {
        FrameGraphResourceID staging_buffer_id;
        FrameGraphResourceID vertex_buffer_id;
        FrameGraphResourceID index_buffer_id;
    };

    struct UploadOffsets {
        core::ByteConstSpan vertices;
        core::ByteConstSpan indices;

        core::ArraySize vertex_offset = 0u;
        core::ArraySize index_offset  = 0u;
    };

    const auto need_to_merge_vert_indices = [this]() {
        return core::ranges::any_of(m_draws, [](const auto &entry) { return !entry.merged; });
    }();

    const auto vertex_buffer_name = core::format("{}:{}", m_vertex_buffer_name, m_frame_counter);
    const auto index_buffer_name  = core::format("{}:{}", m_index_buffer_name, m_frame_counter);

    if (!need_to_merge_vert_indices) {
        auto &vertex_buffer = m_vertex_buffer_pool.get();
        auto &index_buffer  = m_index_buffer_pool.get();
        const auto vertex_buffer_descriptor =
            BufferDescriptor { .size              = vertex_buffer.size(),
                               .usage             = vertex_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_vertex_buffer_id = frame_graph.setRetainedResource(vertex_buffer_name,
                                                             vertex_buffer_descriptor,
                                                             vertex_buffer);

        const auto index_buffer_descriptor =
            BufferDescriptor { .size              = index_buffer.size(),
                               .usage             = index_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_index_buffer_id = frame_graph.setRetainedResource(index_buffer_name,
                                                            index_buffer_descriptor,
                                                            index_buffer);

        return;
    }

    const auto &device  = engine().device();
    const auto &manager = m_manager.get();

    auto offsets            = std::vector<UploadOffsets> {};
    auto vertex_buffer_size = 0u;
    auto index_buffer_size  = 0u;

    for (auto &draw_entry : m_draws) {
        const auto &drawable_component = manager.getComponent<DrawableComponent>(
            m_draw_instances[draw_entry.draw_instances.front()].entity);
        const auto &drawable   = *drawable_component.drawable;
        const auto vertex_size = drawable.vertexSize();

        for (const auto &sub_drawable : drawable.subDrawables()) {
            auto vertex_buffer_size_sub = 0;
            auto index_buffer_size_sub  = 0;
            for (const auto &primitive : sub_drawable.primitives) {
                const auto indices_byte_span = core::toConstByteSpan(primitive.indices);
                offsets.emplace_back(UploadOffsets { primitive.vertices.data(),
                                                     indices_byte_span,
                                                     vertex_buffer_size + vertex_buffer_size_sub,
                                                     index_buffer_size + index_buffer_size_sub });

                m_draw_commands.emplace_back(gsl::narrow_cast<core::UInt32>(
                                                 std::size(primitive.indices)),
                                             std::size(draw_entry.draw_instances),
                                             primitive.first_index,
                                             gsl::narrow_cast<core::Int32>(vertex_buffer_size) /
                                                 vertex_size,
                                             0u);

                vertex_buffer_size_sub += std::size(primitive.vertices.data());
                index_buffer_size_sub += std::size(indices_byte_span);
            }
            vertex_buffer_size += vertex_buffer_size_sub;
            index_buffer_size += index_buffer_size_sub;
        }

        draw_entry.merged = true;
    }

    const auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

    if (!m_vertex_buffer_pool.empty()) m_vertex_buffer_pool.next();
    m_vertex_buffer_pool.emplace(device,
                                 render::HardwareBufferUsage::Vertex |
                                     render::HardwareBufferUsage::Transfert_Dst,
                                 vertex_buffer_size,
                                 render::MemoryProperty::Device_Local);

    if (!m_index_buffer_pool.empty()) m_index_buffer_pool.next();
    m_index_buffer_pool.emplace(device,
                                render::HardwareBufferUsage::Index |
                                    render::HardwareBufferUsage::Transfert_Dst,
                                index_buffer_size,
                                render::MemoryProperty::Device_Local);

    auto &vertex_buffer = m_vertex_buffer_pool.get();
    device.setObjectName(vertex_buffer, vertex_buffer_name);

    auto &index_buffer = m_index_buffer_pool.get();
    device.setObjectName(index_buffer, index_buffer_name);

    const auto vertex_buffer_descriptor =
        BufferDescriptor { .size              = vertex_buffer.size(),
                           .usage             = vertex_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_vertex_buffer_id = frame_graph.setRetainedResource(vertex_buffer_name,
                                                         vertex_buffer_descriptor,
                                                         vertex_buffer);

    const auto index_buffer_descriptor =
        BufferDescriptor { .size              = index_buffer.size(),
                           .usage             = index_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_index_buffer_id =
        frame_graph.setRetainedResource(index_buffer_name, index_buffer_descriptor, index_buffer);

    const auto staging_buffer_descriptor =
        BufferDescriptor { .size              = staging_buffer_size,
                           .usage             = render::HardwareBufferUsage::Transfert_Src,
                           .memory_properties = render::MemoryProperty::Host_Visible |
                                                render::MemoryProperty::Host_Coherent };

    auto &update_vertices_and_indices_pass = frame_graph.addPassNode<UpdateVerticesAndIndicesPass>(
        m_update_vertices_and_indices_pass_name,
        [this, &staging_buffer_descriptor](auto &pass_data, FrameGraphBuilder &builder) {
            pass_data.staging_buffer_id =
                builder.create(core::format("{}:{}",
                                           m_update_vertices_and_indices_pass_name,
                                           "StagingBuffer"),
                               staging_buffer_descriptor);
            pass_data.vertex_buffer_id = builder.write(m_vertex_buffer_id);
            pass_data.index_buffer_id  = builder.write(m_index_buffer_id);
        },
        [vertex_buffer_size,
         index_buffer_size,
         offsets,
         &frame_graph]([[maybe_unused]] const auto &step_data, const auto &pass_data, auto &cmb) {
            auto &staging_buffer = frame_graph.getPhysicalBuffer(pass_data.staging_buffer_id);
            auto &vertex_buffer  = frame_graph.getPhysicalBuffer(pass_data.vertex_buffer_id);
            auto &index_buffer   = frame_graph.getPhysicalBuffer(pass_data.index_buffer_id);

            for (const auto &offset : offsets) {
                staging_buffer.upload(offset.vertices, offset.vertex_offset);
                staging_buffer.upload(offset.indices, vertex_buffer_size + offset.index_offset);
            }
            staging_buffer.flush(0u, vertex_buffer_size + index_buffer_size);

            cmb.copyBuffer(staging_buffer, vertex_buffer, vertex_buffer_size, 0u, 0u);
            cmb.copyBuffer(staging_buffer, index_buffer, index_buffer_size, vertex_buffer_size, 0u);
        },
        QueueFlag::Async_Transfert);

    update_vertices_and_indices_pass.setCullImmune(true);

    m_need_update_draw_data = true;
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::updateDrawDataBuffer(FrameGraph &frame_graph) -> void {
    struct UpdateDrawDataPass {
        FrameGraphResourceID staging_buffer_id;
        FrameGraphResourceID draw_data_buffer_id;
    };

    const auto draw_data_buffer_name =
        core::format("{}:{}", m_draw_data_buffer_name, m_frame_counter);

    if (!m_need_update_draw_data) {
        auto &draw_data_buffer = m_draw_data_buffer_pool.get();

        const auto draw_data_buffer_descriptor =
            BufferDescriptor { .size              = draw_data_buffer.size(),
                               .usage             = draw_data_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_draw_data_buffer_id = frame_graph.setRetainedResource(draw_data_buffer_name,
                                                                draw_data_buffer_descriptor,
                                                                draw_data_buffer);
        return;
    }

    const auto &device             = engine().device();
    const auto staging_buffer_size = std::size(m_draw_data) * sizeof(DrawData);

    if (!m_draw_data_buffer_pool.empty()) m_draw_data_buffer_pool.next();
    m_draw_data_buffer_pool.emplace(device,
                                    render::HardwareBufferUsage::Vertex |
                                        render::HardwareBufferUsage::Transfert_Dst,
                                    staging_buffer_size,
                                    render::MemoryProperty::Device_Local);

    auto &draw_data_buffer = m_draw_data_buffer_pool.get();
    device.setObjectName(draw_data_buffer, draw_data_buffer_name);

    const auto draw_data_buffer_descriptor =
        BufferDescriptor { .size              = draw_data_buffer.size(),
                           .usage             = draw_data_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_draw_data_buffer_id = frame_graph.setRetainedResource(draw_data_buffer_name,
                                                            draw_data_buffer_descriptor,
                                                            draw_data_buffer);

    auto &update_draw_data_pass = frame_graph.addPassNode<UpdateDrawDataPass>(
        m_update_draw_data_buffer_pass_name,
        [this, &staging_buffer_size](auto &pass_data, FrameGraphBuilder &builder) {
            const auto staging_buffer_descriptor =
                BufferDescriptor { .size              = staging_buffer_size,
                                   .usage             = render::HardwareBufferUsage::Transfert_Src,
                                   .memory_properties = render::MemoryProperty::Host_Visible |
                                                        render::MemoryProperty::Host_Coherent };

            pass_data.staging_buffer_id =
                builder.create(core::format("{}:{}",
                                           m_update_draw_data_buffer_pass_name,
                                           "StagingBuffer"),
                               staging_buffer_descriptor);
            pass_data.draw_data_buffer_id = builder.write(m_draw_data_buffer_id);
        },
        [this, staging_buffer_size, &frame_graph]([[maybe_unused]] const auto &step_data,
                                                  const auto &pass_data,
                                                  auto &cmb) {
            auto &staging_buffer   = frame_graph.getPhysicalBuffer(pass_data.staging_buffer_id);
            auto &draw_data_buffer = frame_graph.getPhysicalBuffer(pass_data.draw_data_buffer_id);

            staging_buffer.upload(core::toConstByteSpan(m_draw_data));
            staging_buffer.flush(0u, staging_buffer_size);

            cmb.copyBuffer(staging_buffer, draw_data_buffer, staging_buffer_size, 0u, 0u);
        },
        QueueFlag::Async_Transfert);
    update_draw_data_pass.setCullImmune(true);

    m_need_update_draw_data = false;
}

/////////////////////////////////////
/////////////////////////////////////
void RenderQueue::updateDrawCommandsBuffer(FrameGraph &frame_graph) {
    struct UpdateDrawCommandPass {
        FrameGraphResourceID staging_buffer_id;
        FrameGraphResourceID draw_command_buffer_id;
    };

    const auto draw_command_buffer_name =
        core::format("{}:{}", m_draw_command_buffer_name, m_frame_counter);

    if (!m_need_update_draw_commands) {
        auto &draw_command_buffer = m_draw_command_buffer_pool.get();

        const auto draw_command_buffer_descriptor =
            BufferDescriptor { .size              = draw_command_buffer.size(),
                               .usage             = draw_command_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_draw_command_buffer_id = frame_graph.setRetainedResource(draw_command_buffer_name,
                                                                   draw_command_buffer_descriptor,
                                                                   draw_command_buffer);
        return;
    }

    const auto &device = engine().device();
    const auto staging_buffer_size =
        std::size(m_draw_commands) * sizeof(vk::DrawIndexedIndirectCommand);

    if (!m_draw_command_buffer_pool.empty()) m_draw_command_buffer_pool.next();
    m_draw_command_buffer_pool.emplace(device,
                                       render::HardwareBufferUsage::Indirect |
                                           render::HardwareBufferUsage::Transfert_Dst,
                                       staging_buffer_size,
                                       render::MemoryProperty::Device_Local);

    auto &draw_command_buffer = m_draw_command_buffer_pool.get();
    device.setObjectName(draw_command_buffer, draw_command_buffer_name);

    const auto draw_command_buffer_descriptor =
        BufferDescriptor { .size              = draw_command_buffer.size(),
                           .usage             = draw_command_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_draw_command_buffer_id = frame_graph.setRetainedResource(draw_command_buffer_name,
                                                               draw_command_buffer_descriptor,
                                                               draw_command_buffer);

    auto &update_draw_command_pass = frame_graph.addPassNode<UpdateDrawCommandPass>(
        m_update_draw_command_buffer_pass_name,
        [this, &staging_buffer_size](auto &pass_data, FrameGraphBuilder &builder) {
            const auto staging_buffer_descriptor =
                BufferDescriptor { .size              = staging_buffer_size,
                                   .usage             = render::HardwareBufferUsage::Transfert_Src,
                                   .memory_properties = render::MemoryProperty::Host_Visible |
                                                        render::MemoryProperty::Host_Coherent };

            pass_data.staging_buffer_id =
                builder.create(core::format("{}:{}",
                                           m_update_draw_command_buffer_pass_name,
                                           "StagingBuffer"),
                               staging_buffer_descriptor);
            pass_data.draw_command_buffer_id = builder.write(m_draw_command_buffer_id);
        },
        [this, staging_buffer_size, &frame_graph]([[maybe_unused]] const auto &step_data,
                                                  const auto &pass_data,
                                                  auto &cmb) {
            auto &staging_buffer = frame_graph.getPhysicalBuffer(pass_data.staging_buffer_id);
            auto &draw_command_buffer =
                frame_graph.getPhysicalBuffer(pass_data.draw_command_buffer_id);

            staging_buffer.upload(core::toConstByteSpan(m_draw_commands));
            staging_buffer.flush(0u, staging_buffer_size);

            cmb.copyBuffer(staging_buffer, draw_command_buffer, staging_buffer_size, 0u, 0u);
        },
        QueueFlag::Async_Transfert);
    update_draw_command_pass.setCullImmune(true);

    m_need_update_draw_commands = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderQueue::updateTransformBuffer(FrameGraph &frame_graph) -> void {
    struct UpdateTransformPass {
        FrameGraphResourceID staging_buffer_id;
        FrameGraphResourceID transform_buffer_id;
    };

    auto reupload = false;

    const auto &manager = m_manager.get();

    for (const auto &draw_entry : m_draws) {
        for (const auto &draw_instance_id : draw_entry.draw_instances) {
            const auto &draw_instance = m_draw_instances[draw_instance_id];

            if (!manager.hasComponent<TransformComponent>(draw_instance.entity)) continue;

            const auto &transform_component =
                manager.getComponent<TransformComponent>(draw_instance.entity);

            const auto &draw_data = m_draw_data[draw_instance.draw_data];

            if (transform_component.transform.dirty()) {
                m_transforms[draw_data.transform_id] = transform_component.transform.data();
                reupload                             = true;
            }
        }
    }

    const auto transform_buffer_name =
        core::format("{}:{}", m_transform_buffer_name, m_frame_counter);

    if (!reupload) {
        auto &transform_buffer = m_transform_buffer_pool.get();

        const auto transform_buffer_descriptor =
            BufferDescriptor { .size              = transform_buffer.size(),
                               .usage             = transform_buffer.usage(),
                               .memory_properties = render::MemoryProperty::Device_Local };

        m_transform_buffer_id = frame_graph.setRetainedResource(transform_buffer_name,
                                                                transform_buffer_descriptor,
                                                                transform_buffer);

        return;
    }

    auto &engine       = this->engine();
    const auto &device = engine.device();

    const auto staging_buffer_size = std::size(m_transforms) * sizeof(Transform::Data);

    if (!m_transform_buffer_pool.empty()) m_transform_buffer_pool.next();

    m_transform_buffer_pool.emplace(device,
                                    render::HardwareBufferUsage::Storage |
                                        render::HardwareBufferUsage::Transfert_Dst,
                                    staging_buffer_size,
                                    render::MemoryProperty::Device_Local);

    auto &transform_buffer = m_transform_buffer_pool.get();
    device.setObjectName(transform_buffer, transform_buffer_name);

    const auto transform_buffer_descriptor =
        BufferDescriptor { .size              = transform_buffer.size(),
                           .usage             = transform_buffer.usage(),
                           .memory_properties = render::MemoryProperty::Device_Local };

    m_transform_buffer_id = frame_graph.setRetainedResource(transform_buffer_name,
                                                            transform_buffer_descriptor,
                                                            transform_buffer);

    auto &update_transform_pass = frame_graph.addPassNode<UpdateTransformPass>(
        m_update_transform_buffer_pass_name,
        [this, &staging_buffer_size](auto &pass_data, FrameGraphBuilder &builder) {
            const auto staging_buffer_descriptor =
                BufferDescriptor { .size              = staging_buffer_size,
                                   .usage             = render::HardwareBufferUsage::Transfert_Src,
                                   .memory_properties = render::MemoryProperty::Host_Visible |
                                                        render::MemoryProperty::Host_Coherent };

            pass_data.staging_buffer_id =
                builder.create(core::format("{}:{}",
                                           m_update_transform_buffer_pass_name,
                                           "StagingBuffer"),
                               staging_buffer_descriptor);
            pass_data.transform_buffer_id = builder.write(m_transform_buffer_id);
        },
        [this, staging_buffer_size, &frame_graph]([[maybe_unused]] const auto &step_data,
                                                  const auto &pass_data,
                                                  auto &cmb) {
            auto &staging_buffer   = frame_graph.getPhysicalBuffer(pass_data.staging_buffer_id);
            auto &transform_buffer = frame_graph.getPhysicalBuffer(pass_data.transform_buffer_id);

            staging_buffer.upload(core::toConstByteSpan(m_transforms));
            staging_buffer.flush(0u, staging_buffer_size);

            cmb.copyBuffer(staging_buffer, transform_buffer, staging_buffer_size, 0u, 0u);
        },
        QueueFlag::Async_Transfert);

    update_transform_pass.setCullImmune(true);
}
