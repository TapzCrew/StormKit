// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stack>

#include <stormkit/gpu/core/CommandBuffer.hpp>
#include <stormkit/gpu/core/Device.hpp>
#include <stormkit/gpu/core/Queue.hpp>
#include <stormkit/gpu/pipeline/Framebuffer.hpp>
#include <stormkit/gpu/pipeline/RenderPass.hpp>
#include <stormkit/gpu/resource/Buffer.hpp>
#include <stormkit/gpu/resource/Image.hpp>
#include <stormkit/gpu/resource/ImageView.hpp>

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/render/Renderer.hpp>
#include <stormkit/engine/render/framegraph/BakedFrameGraph.hpp>
#include <stormkit/engine/render/framegraph/FrameGraphBuilder.hpp>
#include <stormkit/engine/render/framegraph/GraphTaskBuilder.hpp>

//#define STORMKIT_RENDERER_MULTITHREADED

#ifdef STORMKIT_RENDERER_MULTITHREADED
    #define LOCK(mutex) auto lock = std::unique_lock { mutex };
#endif

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::FrameGraphBuilder(Engine& engine) : EngineObject { engine } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::~FrameGraphBuilder() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::FrameGraphBuilder(FrameGraphBuilder&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::operator=(FrameGraphBuilder&& other) noexcept
        -> FrameGraphBuilder& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::reset() -> void {
        m_resources.clear();
        m_tasks.clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::bake() -> void {
#ifdef STORMKIT_RENDERER_MULTITHREADED
        auto& thread_pool = engine().threadPool();
        m_bake_future     = thread_pool.postTask<void>([this]() {
#endif
            for (auto& task : m_tasks)
                task->m_ref_count = std::size(task->creates()) + std::size(task->writes());

            for (auto& resource : m_resources)
                resource->m_ref_count = std::size(resource->readers());

            cullUnreferencedResources();
            buildPhysicalDescriptions();
#ifdef STORMKIT_RENDERER_MULTITHREADED
        });
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::createFrameGraph(BakedFrameGraph *old) -> BakedFrameGraph {
#ifdef STORMKIT_RENDERER_MULTITHREADED
        if (m_bake_future.valid()) [[unlikely]]
            m_bake_future.wait();
#endif
        auto data = allocatePhysicalResources();

        return BakedFrameGraph { engine(), *this, std::move(data), old };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::allocateFrameGraph(BakedFrameGraph *old) -> BakedFrameGraphOwnedPtr {
#ifdef STORMKIT_RENDERER_MULTITHREADED
        if (m_bake_future.valid()) [[unlikely]]
            m_bake_future.wait();
#endif
        auto data = allocatePhysicalResources();

        return std::make_unique<BakedFrameGraph>(engine(), *this, std::move(data), old);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::prepareTask(GraphTaskBase& task) noexcept -> void {
        auto task_builder = GraphTaskBuilder { task, *this };
        task.setup(task_builder);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::cullUnreferencedResources() noexcept -> void {
        auto unreferenced_resources = std::stack<GraphResourceBase *> {};
        for (auto& resource : m_resources)
            if (resource->refCount() == 0 && resource->transient())
                unreferenced_resources.push(resource.get());

        while (!std::empty(unreferenced_resources)) {
            auto resource = unreferenced_resources.top();
            unreferenced_resources.pop();

            auto& creator = getTask(resource->creator());
            if (creator.refCount() > 0) --creator.m_ref_count;

            if (creator.m_ref_count == 0 && !creator.cullImune()) {
                for (const auto id : creator.reads()) {
                    auto& read_resource = getResource(id);

                    if (read_resource.refCount() > 0) --read_resource.m_ref_count;

                    if (read_resource.refCount() == 0 && read_resource.transient())
                        unreferenced_resources.push(&read_resource);
                }
            }

            for (const auto id : creator.writes()) {
                auto& writer = getTask(id);

                if (writer.refCount() > 0) --writer.m_ref_count;

                if (writer.refCount() == 0 && !writer.cullImune()) {
                    for (const auto _id : writer.reads()) {
                        auto& read_resource = getResource(_id);

                        if (read_resource.refCount() > 0) --read_resource.m_ref_count;

                        if (read_resource.refCount() == 0 && read_resource.transient())
                            unreferenced_resources.push(&read_resource);
                    }
                }
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildPhysicalDescriptions() noexcept -> void {
        auto layouts              = core::HashMap<GraphID, gpu::ImageLayout> {};
        m_preprocessed_framegraph = core::transformIf(
            m_tasks,
            [](const auto& task) { return !(task->refCount() == 0 && !task->cullImune()); },
            [&, this](const auto& task) {
                return Pass { .id          = task->id(),
                              .description = buildRenderPassPhysicalDescription(*task, layouts),
                              .name        = task->name(),
                              .buffers     = buildBufferPhysicalDescriptions(*task),
                              .images      = buildImagePhysicalDescriptions(*task) };
            });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildImagePhysicalDescriptions(const GraphTaskBase& task) noexcept
        -> std::vector<ImageInfo> {
        const auto images = core::transformIf(
            task.creates(),
            [this](const auto resource_id) {
                const auto& _resource = getResource(resource_id);

                return _resource.transient() && core::is<const GraphImage>(_resource);
            },
            [this](const auto resource_id) { return &getResource<ImageDescription>(resource_id); });

        return core::transform(images, [](const auto& resource) {
            const auto& description = resource->description();

            auto usages = gpu::ImageUsageFlag::Transfert_Src;

            if (gpu::isDepthStencilFormat(description.format))
                usages |= gpu::ImageUsageFlag::Depth_Stencil_Attachment;
            else
                usages |= gpu::ImageUsageFlag::Color_Attachment;

            auto clear_value = gpu::ClearValue {};
            if (gpu::isDepthStencilFormat(description.format))
                clear_value = gpu::ClearDepthStencil {};
            else
                clear_value = gpu::ClearColor { core::RGBColorDef::Black<float> };

            return ImageInfo { .id          = resource->id(),
                               .create_info = { .extent = description.extent,
                                                .format = description.format,
                                                .layers = description.layers,
                                                .type   = description.type,
                                                .usages = usages },
                               .clear_value = clear_value,
                               .name        = resource->name() };
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildBufferPhysicalDescriptions(const GraphTaskBase& task) noexcept
        -> std::vector<BufferInfo> {
        const auto buffers = core::transformIf(
            task.creates(),
            [this](const auto resource_id) {
                const auto& _resource = getResource(resource_id);

                return _resource.transient() && core::is<const GraphBuffer>(_resource);
            },
            [this](const auto resource_id) {
                return &getResource<BufferDescription>(resource_id);
            });

        return core::transform(buffers, [this](const auto& resource) {
            const auto& description = resource->description();

            auto usages = gpu::BufferUsageFlag::Transfert_Src | gpu::BufferUsageFlag::Storage;

            return BufferInfo { .id          = resource->id(),
                                .create_info = { .usages = usages,
                                                 .size   = description.size,
                                                 .property =
                                                     gpu::MemoryPropertyFlag::Device_Local },
                                .name        = resource->name() };
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildRenderPassPhysicalDescription(
        const GraphTaskBase& task,
        core::HashMap<GraphID, gpu::ImageLayout>& layouts) noexcept -> gpu::RenderPassDescription {
        // auto depth_stencil = AttachmentDescriptionPtr { nullptr };
        auto output = gpu::RenderPassDescription {};

        auto creates = core::copyIf(task.creates(), [this](const auto id) noexcept {
            const auto& resource = getResource(id);
            return core::is<GraphImage>(resource);
        });
        auto reads   = core::copyIf(task.reads(), [this](const auto id) noexcept {
            const auto  &resource = getResource(id);
            return core::is<GraphImage>(resource);
        });
        auto writes  = core::copyIf(task.writes(), [this](const auto id) noexcept {
            const auto &resource = getResource(id);
            return core::is<GraphImage>(resource);
        });

        output.attachments = core::transform(creates, [&, this](const auto id) noexcept {
            const auto& resource    = getResource<ImageDescription>(id);
            const auto& description = resource.description();

            auto attachment_description = gpu::AttachmentDescription {
                .format = description.format,

                .load_op            = gpu::AttachmentLoadOperation::Clear,
                .store_op           = gpu::AttachmentStoreOperation::Store,
                .stencil_load_op    = gpu::AttachmentLoadOperation::Dont_Care,
                .stencil_store_op   = gpu::AttachmentStoreOperation::Dont_Care,
                .source_layout      = gpu::ImageLayout::Undefined,
                .destination_layout = gpu::ImageLayout::Color_Attachment_Optimal
            };

            if (gpu::isDepthStencilFormat(description.format)) [[unlikely]] {
                std::swap(attachment_description.load_op, attachment_description.stencil_load_op);
                std::swap(attachment_description.store_op, attachment_description.stencil_store_op);
                attachment_description.destination_layout =
                    gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
            }

            layouts[id] = attachment_description.destination_layout;

            return attachment_description;
        });

        auto to_remove = std::vector<GraphID> {};
        std::ranges::transform(reads,
                               std::back_inserter(output.attachments),
                               [&, this](const auto id) {
                                   const auto& resource    = getResource<ImageDescription>(id);
                                   const auto& description = resource.description();

                                   auto attachment_description = gpu::AttachmentDescription {
                                       .format = description.format,

                                       .load_op          = gpu::AttachmentLoadOperation::Load,
                                       .store_op         = gpu::AttachmentStoreOperation::Dont_Care,
                                       .stencil_load_op  = gpu::AttachmentLoadOperation::Dont_Care,
                                       .stencil_store_op = gpu::AttachmentStoreOperation::Dont_Care,
                                       .source_layout    = layouts.at(id),
                                       .destination_layout = layouts.at(id)
                                   };

                                   if (std::ranges::any_of(writes,
                                                           core::binaryToUnary(id,
                                                                               std::equal_to {}))) {
                                       to_remove.emplace_back(id);
                                       attachment_description.store_op =
                                           gpu::AttachmentStoreOperation::Store;
                                   }

                                   if (gpu::isDepthStencilFormat(description.format)) [[unlikely]] {
                                       std::swap(attachment_description.load_op,
                                                 attachment_description.stencil_load_op);
                                       std::swap(attachment_description.store_op,
                                                 attachment_description.stencil_store_op);
                                       attachment_description.destination_layout =
                                           gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
                                   }

                                   layouts[id] = attachment_description.destination_layout;

                                   return attachment_description;
                               });

        auto ret = std::ranges::remove_if(writes, [&writes](const auto id) {
            return std::ranges::any_of(writes, core::binaryToUnary(id, std::equal_to {}));
        });

        writes.erase(ret.begin(), ret.end());

        std::ranges::transform(writes,
                               std::back_inserter(output.attachments),
                               [&, this](const auto id) {
                                   const auto& resource    = getResource<ImageDescription>(id);
                                   const auto& description = resource.description();

                                   auto attachment_description = gpu::AttachmentDescription {
                                       .format = description.format,

                                       .load_op          = gpu::AttachmentLoadOperation::Clear,
                                       .store_op         = gpu::AttachmentStoreOperation::Store,
                                       .stencil_load_op  = gpu::AttachmentLoadOperation::Dont_Care,
                                       .stencil_store_op = gpu::AttachmentStoreOperation::Dont_Care,
                                       .source_layout    = layouts.at(id),
                                       .destination_layout = layouts.at(id)
                                   };

                                   if (gpu::isDepthStencilFormat(description.format)) [[unlikely]] {
                                       std::swap(attachment_description.load_op,
                                                 attachment_description.stencil_load_op);
                                       std::swap(attachment_description.store_op,
                                                 attachment_description.stencil_store_op);
                                       attachment_description.destination_layout =
                                           gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
                                   }

                                   layouts[id] = attachment_description.destination_layout;

                                   return attachment_description;
                               });

        // TODO support multiple subpasses
        auto i = 0u;
        auto subpass =
            gpu::Subpass { .bind_point = (task.type() == GraphTaskBase::Type::Graphics)
                                             ? gpu::PipelineBindPoint::Graphics
                                             : gpu::PipelineBindPoint::Compute,
                           .attachment_refs =
                               core::transform(output.attachments, [&i](const auto& attachment) {
                                   return gpu::Subpass::Ref { .attachment_id = i++,
                                                              .layout =
                                                                  attachment.destination_layout };
                               }) };
        output.subpasses = { std::move(subpass) };

        return output;
    }

    auto FrameGraphBuilder::allocatePhysicalResources() -> BakedFrameGraph::Data {
        using Data = BakedFrameGraph::Data;

        auto output = Data {};

        const auto& device   = engine().renderer().device();
        auto& graphics_queue = device.graphicsQueue();
        auto& compute_queue  = graphics_queue;

#ifdef STORMKIT_RENDERER_MULTITHREADED
        auto& thread_pool = engine().threadPool();

        auto buffer_mutex         = std::mutex {};
        auto image_mutex          = std::mutex {};
        auto graphics_queue_mutex = std::mutex {};
        auto& compute_queue_mutex = graphics_queue_mutex;

    #define LOCK_BUFFER         LOCK(buffer_mutex)
    #define LOCK_IMAGE          LOCK(image_mutex)
    #define LOCK_GRAPHICS_QUEUE LOCK(graphics_queue_mutex)
    #define LOCK_COMPUTE_QUEUE  LOCK(compute_queue_mutex)

        auto future = core::parallelTransform(
            thread_pool,
#else

    #define LOCK_BUFFER
    #define LOCK_IMAGE
    #define LOCK_GRAPHICS_QUEUE
    #define LOCK_COMPUTE_QUEUE

        output.tasks = core::transform(
#endif
            m_preprocessed_framegraph,
            [&](const auto& pass) {
                auto task = Data::Task {};
                task.id   = pass.id;

                for (const auto& info : pass.buffers) {
                    LOCK_BUFFER
                    auto& buffer =
                        output.buffers.emplace_back(device.createBuffer(info.create_info));
                    device.setObjectName(buffer, core::format("FrameGraph:{}", info.name));
                }

                auto extent      = core::ExtentU {};
                auto attachments = std::vector<gpu::ImageViewConstRef> {};
                for (const auto& info : pass.images) {
                    LOCK_BUFFER

                    extent.width  = std::max(info.create_info.extent.width, extent.width);
                    extent.height = std::max(info.create_info.extent.height, extent.height);

                    auto& image = output.images.emplace_back(device.createImage(info.create_info));
                    device.setObjectName(image, core::format("FrameGraph:{}", info.name));
                    auto& image_view = output.image_views.emplace_back(image.createView());
                    device.setObjectName(image_view, core::format("FrameGraph:{}View", info.name));

                    attachments.emplace_back(std::cref(image_view));

                    task.clear_values.emplace_back(info.clear_value);
                }

                if (pass.description) { // TODO support of async Compute and Transfert queue
                    task.renderpass = device.allocateRenderPass(*pass.description);
                    device.setObjectName(*task.renderpass,
                                         core::format("{}:RenderPass", pass.name));
                    task.framebuffer = task.renderpass->allocateFramebuffer(extent, attachments);
                    device.setObjectName(*task.framebuffer,
                                         core::format("{}:FrameBuffer", pass.name));

                    {
                        LOCK_GRAPHICS_QUEUE

                        task.commandbuffer = graphics_queue.allocateCommandBuffer(
                            gpu::CommandBufferLevel::Secondary);
                    }
                } else {
                    LOCK_COMPUTE_QUEUE

                    task.commandbuffer =
                        compute_queue.allocateCommandBuffer(gpu::CommandBufferLevel::Secondary);
                }

                device.setObjectName(*task.commandbuffer,
                                     core::format("FrameGraph:{}:CommandBuffer", pass.name));

                auto& graph_task = getTask(pass.id);

                task.commandbuffer->begin(false,
                                          gpu::InheritanceInfo { task.renderpass.get(),
                                                                 0,
                                                                 task.framebuffer.get() });
                graph_task.execute(task.renderpass.get(), *task.commandbuffer);
                task.commandbuffer->end();

                return task;
            });
#ifdef STORMKIT_RENDERER_MULTITHREADED
        output.tasks = std::move(future.get());
#endif

        return output;
    } // namespace stormkit::engine
} // namespace stormkit::engine
