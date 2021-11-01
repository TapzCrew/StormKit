/////////// - StormKit::render - ///////////
#include <storm/render/pipeline/Framebuffer.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

#include <storm/engine/render/framegraph/FrameGraph.hpp>
#include <storm/engine/render/framegraph/FrameGraphNode.hpp>
#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

using namespace storm;
using namespace storm::engine;

namespace std {
    auto hash<FrameGraph::Graph>::operator()(const FrameGraph::Graph &graph) const noexcept
        -> core::Hash64 {
        return 0u;
    }
} // namespace std

template<class F>
auto make_copyable_function(F &&f) {
    using dF = std::decay_t<F>;
    auto spf = std::make_shared<dF>(std::forward<F>(f));
    return [spf](auto &&...args) -> decltype(auto) { return (*spf)(decltype(args)(args)...); };
}

auto createWriteOnlyAttachment(const FrameGraphResource &resource,
                               details::FrameGraphNodeID current_node)
    -> render::AttachmentDescription {
    const auto &descriptor = std::get<TextureDescriptor>(resource.descriptor());

    const auto is_depth_attachment =
        core::checkFlag(descriptor.usage, render::TextureUsage::Depth_Stencil_Attachment);

    auto load_op            = render::AttachmentLoadOperation::Dont_Care;
    auto store_op           = render::AttachmentStoreOperation::Store;
    auto source_layout      = render::TextureLayout::Undefined;
    auto destination_layout = (is_depth_attachment)
                                  ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                                  : render::TextureLayout::Color_Attachment_Optimal;

    if (current_node == resource.createdBy()) load_op = render::AttachmentLoadOperation::Clear;

    return render::AttachmentDescription { .format             = descriptor.format,
                                           .load_op            = load_op,
                                           .store_op           = store_op,
                                           .source_layout      = source_layout,
                                           .destination_layout = destination_layout };
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(Engine &engine) noexcept : m_engine { engine } {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::~FrameGraph() {
    cleanup();
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(FrameGraph &&other) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::operator=(FrameGraph &&other) noexcept -> FrameGraph & = default;

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::beginNewFrame(render::Surface::Frame &frame) -> void {
    m_current_frame = &frame;

    cleanup();

    m_graph.hash = std::hash<Graph> {}(m_graph);

    static auto _hash      = 0u;
    m_cached_graphs[_hash] = std::move(m_graph);
    _hash++;

    m_timeline.clear();

    m_graph.next_resource_id = FrameGraphResourceID { 0u };
    m_graph.next_pass_id     = details::FrameGraphNodeID { 0u };
    m_graph.backbuffer_id    = FrameGraphResourceID {};
    m_graph.nodes.clear();
    m_graph.resources.clear();
    m_graph.resource_link.clear();
    m_graph.retained_textures.clear();
    m_graph.retained_buffers.clear();
}

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::build() -> void {
    cullNodes();
    buildPhysicalResources();
    buildRenderPasses();
}

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::execute() -> void {
    for (auto &step : m_timeline) {
        auto &cmb  = step.cmb;
        auto &node = m_graph.nodes[step.node.id];

        cmb->begin(true);
        if (step.render_pass) {
            cmb->beginRenderPass(*step.render_pass, *step.frame_buffer, step.clear_values);

            if (step.pipeline) { cmb->bindGraphicsPipeline(*step.pipeline); }
        }

        const auto step_data = FrameGraphStepData { .render_pass  = step.render_pass,
                                                    .frame_buffer = step.frame_buffer,
                                                    .pipeline     = step.pipeline };
        node->execute(step_data, *cmb);

        if (step.render_pass) { cmb->endRenderPass(); }

        for (const auto &barrier : step.barriers) {
            cmb->pipelineBarrier(barrier.src_mask,
                                 barrier.dst_mask,
                                 barrier.dependency,
                                 barrier.memory_barriers,
                                 barrier.buffer_memory_barriers,
                                 barrier.image_memory_barriers);
        }

        cmb->end();
        cmb->build();

        if (step.present_step) {
            cmb->submit(core::makeConstObserverArray(m_current_frame->texture_available),
                        core::makeConstObserverArray(m_current_frame->render_finished),
                        m_current_frame->in_flight);

        } else {
            cmb->submit({}, {}, step.fence);
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::getResource(FrameGraphResourceID id) noexcept -> FrameGraphResource & {
    return *m_graph.resources[id.id];
}

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraph::getResource(FrameGraphResourceID id) const noexcept -> const FrameGraphResource & {
    return *m_graph.resources[id.id];
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::cullNodes() {
    for (auto &node : m_graph.nodes) {
        node->m_ref_count = std::size(node->m_creates) + std::size(node->m_writes);
    }
    for (auto &resource : m_graph.resources) {
        resource->m_ref_count = std::size(resource->m_read_in) + std::size(resource->m_writed_in);
    }

    auto unreferenced_resources = std::stack<FrameGraphResourceRef> {};
    for (auto &resource : m_graph.resources) {
        if (resource->m_ref_count == 0 && resource->isTransient())
            unreferenced_resources.emplace(*resource);
    }

    while (!std::empty(unreferenced_resources)) {
        auto &resource = unreferenced_resources.top().get();
        unreferenced_resources.pop();

        auto &creator = *m_graph.nodes[resource.m_created_by.id];
        if (creator.m_ref_count > 0) creator.m_ref_count--;
        if (creator.m_ref_count == 0 && !creator.isCullImmune()) {
            for (const auto read_id : creator.m_reads) {
                auto &read_resource = *m_graph.resources[read_id.id];
                if (read_resource.m_ref_count > 0) read_resource.m_ref_count--;
                if (read_resource.m_ref_count == 0 && read_resource.isTransient())
                    unreferenced_resources.emplace(read_resource);
            }
        }

        for (const auto write_id : resource.m_writed_in) {
            auto &writer = *m_graph.nodes[write_id.id];

            if (writer.m_ref_count > 0) writer.m_ref_count--;
            if (writer.m_ref_count == 0 && !writer.isCullImmune()) {
                for (const auto read_id : writer.m_reads) {
                    auto &read_resource = *m_graph.resources[read_id.id];
                    if (read_resource.m_ref_count > 0) read_resource.m_ref_count--;
                    if (read_resource.m_ref_count == 0 && read_resource.isTransient())
                        unreferenced_resources.emplace(read_resource);
                }
            }
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::buildPhysicalResources() {
    auto transient_resources = std::vector<FrameGraphResourceConstRef> {};
    transient_resources.reserve(std::size(m_graph.resources));

    for (const auto &resource : m_graph.resources) {
        if (resource->m_ref_count > 0 && resource->isTransient())
            transient_resources.emplace_back(*resource);
    }

    for (const auto &resource : transient_resources) {
        std::visit(core::overload { [id = resource.get().id(), this](const auto &descriptor) {
                       realize(id, descriptor);
                   } },
                   resource.get().descriptor());
    }
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::buildRenderPasses() {
    auto nodes = std::vector<details::FrameGraphNodeConstRef> {};
    nodes.reserve(std::size(m_graph.nodes));

    for (const auto &node : m_graph.nodes) {
        if (node->m_ref_count > 0) nodes.emplace_back(*node);
    }

    auto &cache        = engine().resourceCache();
    const auto &device = engine().device();

    auto last_node_use_backbuffer = details::FrameGraphNodeID {};
    for (const auto &_node : nodes) {
        const auto &node       = _node.get();
        const auto queue_flags = node.queueFlags();

        if (queue_flags == QueueFlag::Graphics) {
            auto cmb = device.graphicsQueue().createCommandBufferPtr();

            auto [render_pass_description, clear_values] = buildRenderPass(node);

            auto backbuffer = core::ranges::find_if(node.m_writes, [this](const auto &res) {
                return res == m_graph.backbuffer_id;
            });

            if (backbuffer != core::ranges::cend(node.m_writes))
                last_node_use_backbuffer = node.id();

            auto &step = m_timeline.emplace_back(
                Step { .node        = node.id(),
                       .cmb         = std::move(cmb),
                       .render_pass = &cache.requestRenderPass(render_pass_description) });

            const auto writes = node.writes();
            auto views        = render::TextureViewConstPtrArray {};
            for (auto id : writes) {
                const auto &resource = getResource(id);
                if (std::holds_alternative<TextureDescriptor>(resource.descriptor())) {
                    auto &texture          = getPhysicalTexture(id);
                    const auto aspect_mask = (render::isDepthFormat(texture.format()))
                                                 ? render::TextureAspectMask::Depth
                                                 : render::TextureAspectMask::Color;
                    auto &view             = m_texture_views.emplace_back(
                        texture.createViewPtr(render::TextureViewType::T2D,
                                              render::TextureSubresourceRange { aspect_mask }));
                    device.setObjectName(*view, core::format("{}:View", getResource(id).name()));

                    views.emplace_back(core::makeConstObserver(view));
                } else if (std::holds_alternative<BufferDescriptor>(resource.descriptor())) {
                    auto src_flag = render::PipelineStageFlag::None;
                    if (node.queueFlags() == QueueFlag::Compute ||
                        node.queueFlags() == QueueFlag::Async_Compute)
                        src_flag = render::PipelineStageFlag::Compute_Shader;
                    else if (node.queueFlags() == QueueFlag::Transfert ||
                             node.queueFlags() == QueueFlag::Async_Transfert)
                        src_flag = render::PipelineStageFlag::Transfer;

                    auto &buffer = getPhysicalBuffer(id);
                    step.barriers.emplace_back(Barrier {
                        src_flag,
                        render::PipelineStageFlag::Vertex_Shader,
                        render::DependencyFlag::None,
                        {},
                        render::BufferMemoryBarriers { { render::AccessFlag::Transfer_Write,
                                                         render::AccessFlag::Shader_Read,
                                                         render::QUEUE_FAMILY_IGNORED,
                                                         render::QUEUE_FAMILY_IGNORED,
                                                         buffer,
                                                         buffer.size(),
                                                         0 } },
                        {} });
                }
            }

            auto &frame_buffer = m_frame_buffers.emplace_back(
                step.render_pass->createFramebufferPtr(node.extent(), std::move(views)));
            step.frame_buffer = core::makeObserver(frame_buffer);
            step.clear_values = std::move(clear_values);

            device.setObjectName(*step.render_pass, core::format("{}:RenderPass", node.name()));
            device.setObjectName(*step.frame_buffer, core::format("{}:FrameBuffer", node.name()));
            if (node.hasGraphicsPipeline()) {
                step.pipeline = core::makeObserver(
                    cache.requestGraphicsPipeline(node.graphicsPipeline(), *step.render_pass));
                device.setObjectName(*step.pipeline,
                                     core::format("{}:GraphicsPipeline", node.name()));
            }
        } else if (queue_flags == QueueFlag::Transfert) {
            auto cmb = device.graphicsQueue().createCommandBufferPtr();

            m_timeline.emplace_back(Step { node.id(), std::move(cmb) });
        } else if (queue_flags == QueueFlag::Async_Transfert) {
            auto &queue = (device.hasAsyncTransfertQueue()) ? device.asyncTransfertQueue()
                                                            : device.graphicsQueue();
            auto cmb    = queue.createCommandBufferPtr();

            m_timeline.emplace_back(Step {
                node.id(),
                std::move(cmb),
            });
        }
    }

    if (last_node_use_backbuffer.id != details::FrameGraphNodeID::INVALID_HANDLE) {
        auto present_step =
            core::ranges::find_if(m_timeline, [&last_node_use_backbuffer](const auto &step) {
                return step.node == last_node_use_backbuffer;
            });

        present_step->present_step = true;
        present_step->fence        = core::makeObserver(m_current_frame->in_flight);
    }

    for (auto &step : m_timeline) {
        if (step.fence == nullptr) {
            auto &fence = m_fences.emplace_back(device.createFencePtr());
            step.fence  = core::makeObserver(fence);
        }
    }
}

std::tuple<render::RenderPassDescription, std::vector<render::ClearValue>>
    FrameGraph::buildRenderPass(const details::FrameGraphNode &node) {
    struct Attachment {
        FrameGraphResourceID res_id;
        core::UInt32 attachment_id;
    };

    auto clear_values = std::vector<render::ClearValue> {};

    const auto reads  = node.reads();
    const auto writes = node.writes();

    auto read_only_attachments  = std::vector<FrameGraphResourceID> {};
    auto read_write_attachments = std::vector<FrameGraphResourceID> {};
    auto write_only_attachments = std::vector<FrameGraphResourceID> {};

    for (const auto read : reads) {
        const auto &resource = getResource(read);
        if (!std::holds_alternative<TextureDescriptor>(resource.descriptor())) continue;

        if (core::ranges::find(writes, read) != core::ranges::end(writes))
            read_write_attachments.emplace_back(read);
        else
            read_only_attachments.emplace_back(read);
    }

    for (const auto write : writes) {
        const auto &resource = getResource(write);
        if (!std::holds_alternative<TextureDescriptor>(resource.descriptor())) continue;

        if (core::ranges::find(read_write_attachments, write) ==
            core::ranges::end(read_write_attachments))
            write_only_attachments.emplace_back(write);
    }

    auto subpasses       = render::Subpasses {};
    auto attachments     = render::AttachmentDescriptions {};
    auto attachments_ids = std::vector<Attachment> {};

    auto &subpass      = subpasses.emplace_back();
    subpass.bind_point = render::PipelineBindPoint::Graphics;

    for (const auto id : write_only_attachments) {
        const auto &resource = getResource(id);

        auto attachment = createWriteOnlyAttachment(resource, node.id());
        if (id == m_graph.backbuffer_id) {
            attachment.load_op            = render::AttachmentLoadOperation::Clear;
            attachment.destination_layout = render::TextureLayout::Present_Src;
        }

        if (attachment.load_op == render::AttachmentLoadOperation::Clear) {
            if (isDepthFormat(attachment.format))
                clear_values.emplace_back(render::ClearDepthStencil {});
            else
                clear_values.emplace_back(render::ClearColor {});
        }

        attachments.emplace_back(attachment);
    }

    auto i = 0u;
    for (const auto &attachment : attachments) {
        const auto subpass_layout = (render::isDepthFormat(attachment.format))
                                        ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                                        : render::TextureLayout::Color_Attachment_Optimal;
        subpass.attachment_refs.emplace_back(render::Subpass::Ref { i, subpass_layout });
        ++i;
    }

    return { render::RenderPassDescription { std::move(attachments), std::move(subpasses) },
             std::move(clear_values) };
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::realize(FrameGraphResourceID id, const TextureDescriptor &descriptor) {
    const auto &device = engine().device();
    auto texture       = device.createTexturePtr(descriptor.extent,
                                           descriptor.format,
                                           1u,
                                           descriptor.levels,
                                           descriptor.type,
                                           render::TextureCreateFlag::None,
                                           descriptor.samples,
                                           descriptor.usage);
    device.setObjectName(*texture, getResource(id).name());

    m_textures.emplace_back(PhysicalTexture { id, std::move(texture) });
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::realize(FrameGraphResourceID id, const BufferDescriptor &descriptor) {
    const auto &device = engine().device();

    auto buffer = device.createHardwareBufferPtr(descriptor.usage,
                                                 descriptor.size,
                                                 descriptor.memory_properties);
    device.setObjectName(*buffer, getResource(id).name());

    m_buffers.emplace_back(PhysicalBuffer { id, std::move(buffer) });
}

void FrameGraph::cleanup() {
    if (m_current_frame == nullptr) return;

    auto &deletion_queue = engine().deletionQueue();

    auto command_buffers = std::vector<render::CommandBufferOwnedPtr> {};
    command_buffers.reserve(std::size(m_timeline));

    for (auto &step : m_timeline) { command_buffers.emplace_back(std::move(step.cmb)); }

    deletion_queue.push(*m_current_frame->in_flight,
                        make_copyable_function([command_buffers { std::move(command_buffers) },
                                                frame_buffers { std::move(m_frame_buffers) },
                                                texture_views { std::move(m_texture_views) },
                                                textures { std::move(m_textures) },
                                                buffers { std::move(m_buffers) }]() mutable {
                            command_buffers.clear();
                            frame_buffers.clear();
                            texture_views.clear();
                            textures.clear();
                            buffers.clear();
                        }));
}
