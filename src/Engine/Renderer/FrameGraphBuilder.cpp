// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Gpu;

import :Renderer.FrameGraph;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::bake() -> void {
        for (auto& task : m_tasks)
            task.m_ref_count = std::size(task.creates()) + std::size(task.writes());

        for (auto& resource : m_resources)
            std::visit([](auto& resource) { resource.m_ref_count = std::size(resource.readers()); },
                       resource);

        cullUnreferencedResources();
        buildPhysicalDescriptions();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::createFrameGraph(const gpu::Device& device, BakedFrameGraph* old)
        -> BakedFrameGraph {
        auto data = allocatePhysicalResources(device);

        return BakedFrameGraph { *this, std::move(data), old };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::allocateFrameGraph(const gpu::Device& device, BakedFrameGraph* old)
        -> std::unique_ptr<BakedFrameGraph> {
        auto data = allocatePhysicalResources(device);

        return std::make_unique<BakedFrameGraph>(*this, std::move(data), old);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::prepareTask(GraphTask& task) noexcept -> void {
        auto task_builder = GraphTaskBuilder { task, *this };
        auto data         = m_datas[task.dataID()];
        task.onSetup(*std::bit_cast<core::Byte*>(&data), task_builder);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::cullUnreferencedResources() noexcept -> void {
        auto unreferenced_resources = std::stack<core::NakedRef<GraphResourceVariant>> {};

        constexpr auto decrementRefcount = [](auto& value) noexcept {
            if (value.m_ref_count > 0) --value.m_ref_count;
        };

        constexpr auto isUnreferenced = [](const auto& resource) noexcept {
            return resource.refCount() == 0 and resource.transient();
        };

        constexpr auto shouldCull = [](const auto& task) noexcept {
            return task.refCount() == 0 and not task.cullImune();
        };

        const auto cull = [&decrementRefcount, &isUnreferenced, &unreferenced_resources, this](
                              auto& task) noexcept {
            for (const auto id : task.reads()) {
                auto& resource = getResource(id);

                if (std::visit(
                        [&decrementRefcount, &isUnreferenced](auto& value) {
                            decrementRefcount(value);
                            return isUnreferenced(value);
                        },
                        resource))
                    unreferenced_resources.push(resource);
            }
        };

        for (auto& resource : m_resources)
            if (std::visit(isUnreferenced, resource)) unreferenced_resources.push(resource);

        while (!std::empty(unreferenced_resources)) {
            auto resource = unreferenced_resources.top();
            unreferenced_resources.pop();

            auto& creator =
                getTask(std::visit([](auto& resource) noexcept { return resource.creator(); },
                                   resource.get()));
            decrementRefcount(creator);

            if (shouldCull(creator)) cull(creator);

            for (const auto id : creator.writes()) {
                auto& writer = getTask(id);

                decrementRefcount(writer);
                if (shouldCull(writer)) cull(writer);
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildPhysicalDescriptions() noexcept -> void {
        auto layouts = core::HashMap<GraphID, gpu::ImageLayout> {};
        m_preprocessed_framegraph =
            m_tasks | std::views::filter([](const auto& task) noexcept {
                return not(task.refCount() == 0 and not task.cullImune());
            }) |
            std::views::transform([&layouts, this](const auto& task) noexcept -> decltype(auto) {
                return Pass { .id         = task.id(),
                              .renderpass = buildRenderPassPhysicalDescription(task, layouts),
                              .name       = task.name(),
                              .buffers    = buildBufferPhysicalDescriptions(task),
                              .images     = buildImagePhysicalDescriptions(task) };
            }) |
            std::ranges::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildImagePhysicalDescriptions(const GraphTask& task) noexcept
        -> std::vector<ImageInfo> {
        return task.creates() | std::views::filter([this](const auto& id) noexcept {
                   const auto& resource = getResource(id);

                   return core::is<GraphImage>(resource) and
                          getResource<ImageDescription>(id).transient();
               }) |
               std::views::transform([this](const auto& id) noexcept -> decltype(auto) {
                   const auto& resource    = getResource<ImageDescription>(id);
                   const auto& description = resource.description();

                   const auto usages = [&description] noexcept {
                       if (gpu::isDepthStencilFormat(description.format))
                           return gpu::ImageUsageFlag::Depth_Stencil_Attachment |
                                  gpu::ImageUsageFlag::Transfert_Src;

                       return gpu::ImageUsageFlag::Color_Attachment |
                              gpu::ImageUsageFlag::Transfert_Src;
                   }();

                   const auto clear_value = [&description] noexcept -> gpu::ClearValue {
                       if (gpu::isDepthStencilFormat(description.format))
                           return gpu::ClearDepthStencil {};

                       return gpu::ClearColor {};
                   }();

                   const auto& name = resource.name();

                   return ImageInfo { .id = id,
                                      .create_info =
                                          gpu::Image::CreateInfo {
                                              .extent = description.extent,
                                              .format = description.format,
                                              .layers = description.layers,
                                              .type   = description.type,
                                              .usages = usages,
                                          },
                                      .clear_value = clear_value,
                                      .name        = name };
               }) |
               std::ranges::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildBufferPhysicalDescriptions(const GraphTask& task) noexcept
        -> std::vector<BufferInfo> {
        return task.creates() | std::views::filter([this](const auto& id) noexcept {
                   const auto& resource = getResource(id);

                   return core::is<GraphBuffer>(resource) and
                          getResource<BufferDescription>(id).transient();
               }) |
               std::views::transform([this](const auto& id) noexcept -> decltype(auto) {
                   const auto& resource    = getResource<BufferDescription>(id);
                   const auto& description = resource.description();

                   const auto usages =
                       gpu::BufferUsageFlag::Transfert_Src | gpu::BufferUsageFlag::Storage;

                   const auto& name = resource.name();

                   return BufferInfo { .id = id,
                                       .create_info =
                                           gpu::Buffer::CreateInfo { .usages = usages,
                                                                     .size   = description.size },
                                       //.setMemoryProperty(gpu::MemoryPropertyFlag::eDeviceLocal),
                                       .name = name };
               }) |
               std::ranges::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildRenderPassPhysicalDescription(
        const GraphTask&                          task,
        core::HashMap<GraphID, gpu::ImageLayout>& layouts) noexcept -> RenderPassData {
        auto to_remove = std::vector<GraphID> {};

        const auto creates =
            task.creates() | std::views::filter([this](const auto resource_id) noexcept {
                const auto& resource = getResource(resource_id);
                return core::is<GraphImage>(resource);
            }) |
            std::views::transform([&, this](const auto id) noexcept {
                const auto& resource    = getResource<ImageDescription>(id);
                const auto& description = resource.description();

                auto attachment_description = gpu::AttachmentDescription {
                    .format             = description.format,
                    .load_op            = gpu::AttachmentLoadOperation::Clear,
                    .store_op           = gpu::AttachmentStoreOperation::Store,
                    .stencil_load_op    = gpu::AttachmentLoadOperation::Dont_Care,
                    .stencil_store_op   = gpu::AttachmentStoreOperation::Dont_Care,
                    .source_layout      = gpu::ImageLayout::Undefined,
                    .destination_layout = gpu::ImageLayout::Color_Attachment_Optimal
                };

                if (isDepthStencilFormat(description.format)) [[unlikely]] {
                    std::swap(attachment_description.load_op,
                              attachment_description.stencil_load_op);
                    std::swap(attachment_description.store_op,
                              attachment_description.stencil_store_op);
                    attachment_description.destination_layout =
                        gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
                }

                layouts[id] = attachment_description.destination_layout;

                return attachment_description;
            }) |
            std::ranges::to<std::vector>();

        const auto writes = task.writes() | std::views::filter([this](const auto id) noexcept {
                                const auto& resource = getResource(id);
                                return core::is<GraphImage>(resource);
                            }) |
                            std::views::transform([&, this](const auto id) {
                                const auto& resource    = getResource<ImageDescription>(id);
                                const auto& description = resource.description();

                                auto attachment_description = gpu::AttachmentDescription {
                                    .format             = description.format,
                                    .load_op            = gpu::AttachmentLoadOperation::Clear,
                                    .store_op           = gpu::AttachmentStoreOperation::Store,
                                    .stencil_load_op    = gpu::AttachmentLoadOperation::Dont_Care,
                                    .stencil_store_op   = gpu::AttachmentStoreOperation::Dont_Care,
                                    .source_layout      = layouts.at(id),
                                    .destination_layout = layouts.at(id)
                                };

                                if (isDepthStencilFormat(description.format)) [[unlikely]] {
                                    std::swap(attachment_description.load_op,
                                              attachment_description.stencil_load_op);
                                    std::swap(attachment_description.store_op,
                                              attachment_description.stencil_store_op);
                                    attachment_description.destination_layout =
                                        gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
                                }

                                layouts[id] = attachment_description.destination_layout;

                                return attachment_description;
                            }) |
                            std::ranges::to<std::vector>();

        const auto reads = task.reads() | std::views::filter([this](const auto id) noexcept {
                               const auto& resource = getResource(id);
                               return core::is<GraphImage>(resource);
                           }) |
                           std::views::transform([&, this](const auto id) {
                               const auto& resource    = getResource<ImageDescription>(id);
                               const auto& description = resource.description();

                               auto attachment_description = gpu::AttachmentDescription {
                                   .format             = description.format,
                                   .load_op            = gpu::AttachmentLoadOperation::Load,
                                   .store_op           = gpu::AttachmentStoreOperation::Dont_Care,
                                   .stencil_load_op    = gpu::AttachmentLoadOperation::Dont_Care,
                                   .stencil_store_op   = gpu::AttachmentStoreOperation::Dont_Care,
                                   .source_layout      = layouts.at(id),
                                   .destination_layout = layouts.at(id)
                               };

                               if (std::ranges::any_of(task.writes(), core::monadic::equal(id))) {
                                   to_remove.emplace_back(id);
                                   attachment_description.store_op =
                                       gpu::AttachmentStoreOperation::Store;
                               }

                               if (isDepthStencilFormat(description.format)) [[unlikely]] {
                                   std::swap(attachment_description.load_op,
                                             attachment_description.stencil_load_op);
                                   std::swap(attachment_description.store_op,
                                             attachment_description.stencil_store_op);
                                   attachment_description.destination_layout =
                                       gpu::ImageLayout::Depth_Stencil_Attachment_Optimal;
                               }

                               layouts[id] = attachment_description.destination_layout;

                               return attachment_description;
                           }) |
                           std::ranges::to<std::vector>();

        auto output = RenderPassData {};
        output.description.attachments =
            core::moveAndConcat(std::move(creates), std::move(reads), std::move(writes));

        auto color_refs = std::vector<gpu::Subpass::Ref> {};
        color_refs.reserve(std::size(output.description.attachments));

        auto depth_attachment_ref = std::optional<gpu::Subpass::Ref> {};
        for (auto&& [i, attachment] : output.description.attachments | std::views::enumerate) {
            if (isDepthFormat(attachment.format))
                depth_attachment_ref =
                    gpu::Subpass::Ref { .attachment_id = core::as<core::UInt32>(i),
                                        .layout        = attachment.destination_layout };
            else
                color_refs.emplace_back(
                    gpu::Subpass::Ref { .attachment_id = core::as<core::UInt32>(i),
                                        .layout        = attachment.destination_layout });
        }

        // TODO support multiple subpasses
        output.description.subpasses.emplace_back(
            gpu::Subpass { .bind_point            = task.type() == GraphTask::Type::Graphics
                                                        ? gpu::PipelineBindPoint::Graphics
                                                        : gpu::PipelineBindPoint::Compute,
                           .color_attachment_refs = std::move(color_refs),
                           .depth_attachment_ref  = std::move(depth_attachment_ref) });

        return output;
    }

    auto FrameGraphBuilder::allocatePhysicalResources(const gpu::Device& device)
        -> BakedFrameGraph::Data {
        using Data = BakedFrameGraph::Data;

        auto output = Data {};
        // output.raster_command_pool = gpu::CommandPool::create(device, device.rasterQueue());
        // output.main_cmb            = output.raster_command_pool.
        // output.blit_cmb            = createCommandBuffer(device, output.raster_command_pool);

        // auto future = core::parallelTransform(
        //     m_thread_pool,
        //     m_preprocessed_framegraph,
        //     [&](const auto& pass) {
        //         auto task = Data::Task {};
        //         task.id   = pass.id;
        //
        //         for (const auto& info : pass.buffers) {
        //             auto create_info = gpu::BufferCreateInfo{}
        //             auto& buffer =
        //                 output.buffers.emplace_back(device.createBuffer(info.create_info));
        //             device.setObjectName(buffer, std::format("FrameGraph:{}", info.name));
        //         }
        //
        //         auto extent      = core::math::ExtentU {};
        //         auto attachments = std::vector<gpu::ImageViewConstRef> {};
        //         for (const auto& info : pass.images) {
        //             extent.width  = std::max(info.create_info.extent.width, extent.width);
        //             extent.height = std::max(info.create_info.extent.height, extent.height);
        //
        //             auto& image =
        //             output.images.emplace_back(device.createImage(info.create_info));
        //             device.setObjectName(image, std::format("FrameGraph:{}", info.name));
        //             auto& image_view = output.image_views.emplace_back(image.createView());
        //             device.setObjectName(image_view, std::format("FrameGraph:{}View",
        //             info.name));
        //
        //             attachments.emplace_back(std::cref(image_view));
        //
        //             task.clear_values.emplace_back(info.clear_value);
        //         }
        //
        //         if (pass.description) { // TODO support of async Compute and Transfert queue
        //             task.renderpass = device.allocateRenderPass(*pass.description);
        //             device.setObjectName(*task.renderpass, std::format("{}:RenderPass",
        //             pass.name)); task.framebuffer =
        //             task.renderpass->allocateFramebuffer(extent, attachments);
        //             device.setObjectName(*task.framebuffer,
        //                                  std::format("{}:FrameBuffer", pass.name));
        //
        //             {
        //                 task.commandbuffer = graphics_queue.allocateCommandBuffer(
        //                     gpu::CommandBufferLevel::Secondary);
        //             }
        //         } else {
        //             task.commandbuffer =
        //                 compute_queue.allocateCommandBuffer(gpu::CommandBufferLevel::Secondary);
        //         }
        //
        //         device.setObjectName(*task.commandbuffer,
        //                              std::format("FrameGraph:{}:CommandBuffer", pass.name));
        //
        //         auto& graph_task = getTask(pass.id);
        //
        //         const auto inheritance_info = gpu::CommandBufferInheritanceInfo {
        //         }
        //         const auto secondary_begin_info =
        //             gpu::CommandBufferBeginInfo {}
        //                 .setIneri
        //
        //                     task.commandbuffer->begin(false,
        //                                               gpu::InheritanceInfo {
        //                                                   task.renderpass.get(),
        //                                                   0,
        //                                                   task.framebuffer.get() });
        //         graph_task.execute(task.renderpass.get(), *task.commandbuffer);
        //         task.commandbuffer->end();
        //
        //         return task;
        //     });
        //
        // output.tasks = std::move(future.get());

        // m_data.main_cmb->begin();
        //
        // for (const auto& task : m_data.tasks) {
        //     if (task.renderpass)
        //         m_data.main_cmb->beginRenderPass(*task.renderpass,
        //                                     *task.framebuffer,
        //                                     task.clear_values,
        //                                     true);
        //
        //     auto command_buffers = core::makeConstObserverStaticArray(task.commandbuffer);
        //     m_main_cmb->executeSubCommandBuffers(command_buffers);
        //
        //     if (task.renderpass) m_main_cmb->endRenderPass();
        // }
        //
        // m_main_cmb->end();
        return output;
    } // namespace stormkit::engine
} // namespace stormkit::engine
