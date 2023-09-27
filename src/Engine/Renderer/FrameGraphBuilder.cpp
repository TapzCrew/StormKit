// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Engine;

import std;

import stormkit.Core;

import vulkan_hpp;

import :Renderer.FrameGraphBuilder;
import :Renderer.BakedFrameGraph;
import :Renderer.GraphTask;
import :Renderer.GraphID;
import :Renderer.GraphResource;
import :Renderer.GraphTaskBuilder;

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
    auto FrameGraphBuilder::createFrameGraph(const vk::raii::Device& device, BakedFrameGraph *old)
        -> BakedFrameGraph {
        auto data = allocatePhysicalResources(device);

        return BakedFrameGraph { *this, std::move(data), old };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::allocateFrameGraph(const vk::raii::Device& device, BakedFrameGraph *old)
        -> std::unique_ptr<BakedFrameGraph> {
        auto data = allocatePhysicalResources(device);

        return std::make_unique<BakedFrameGraph>(*this, std::move(data), old);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::prepareTask(GraphTask& task) noexcept -> void {
        auto task_builder = GraphTaskBuilder { task, *this };
        auto data         = m_datas[task.dataID()];
        task.onSetup(*std::bit_cast<core::Byte *>(&data), task_builder);
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
        auto layouts = core::HashMap<GraphID, vk::ImageLayout> {};
        m_preprocessed_framegraph =
            m_tasks | std::views::filter([](const auto& task) noexcept {
                return not(task.refCount() == 0 and not task.cullImune());
            }) |
            std::views::transform([&layouts, this](const auto& task) noexcept -> decltype(auto) {
                return Pass { .id          = task.id(),
                              .vulkan_data = buildRenderPassPhysicalDescription(task, layouts),
                              .name        = task.name(),
                              .buffers     = buildBufferPhysicalDescriptions(task),
                              .images      = buildImagePhysicalDescriptions(task) };
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

                   const auto usages = [&description]() noexcept {
                       if (isDepthStencilFormat(description.format))
                           return vk::ImageUsageFlagBits::eDepthStencilAttachment |
                                  vk::ImageUsageFlagBits::eTransferSrc;

                       return vk::ImageUsageFlagBits::eColorAttachment |
                              vk::ImageUsageFlagBits::eTransferSrc;
                   }();

                   const auto clear_value = [&description]() noexcept -> decltype(auto) {
                       if (isDepthStencilFormat(description.format))
                           return vk::ClearValue {}.setDepthStencil(vk::ClearDepthStencilValue {});

                       return vk::ClearValue {}.setColor(
                           vk::ClearColorValue { core::RGBColorDef::Black<float> });
                   }();

                   const auto& name = resource.name();

                   return ImageInfo { .id = id,
                                      .create_info =
                                          vk::ImageCreateInfo {}
                                              .setExtent(vk::Extent3D { description.extent.width,
                                                                        description.extent.height,
                                                                        description.extent.depth })
                                              .setFormat(description.format)
                                              .setArrayLayers(description.layers)
                                              .setImageType(description.type)
                                              .setUsage(usages),
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

                   const auto usages = vk::BufferUsageFlagBits::eTransferSrc |
                                       vk::BufferUsageFlagBits::eStorageBuffer;

                   const auto& name = resource.name();

                   return BufferInfo {
                       .id = id,
                       .create_info =
                           vk::BufferCreateInfo {}.setUsage(usages).setSize(description.size),
                       //.setMemoryProperty(vk::MemoryPropertyFlagBits::eDeviceLocal),
                       .name = name
                   };
               }) |
               std::ranges::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::buildRenderPassPhysicalDescription(
        const GraphTask                        & task,
        core::HashMap<GraphID, vk::ImageLayout>& layouts) noexcept -> VulkanRenderPassData {
        auto to_remove = std::vector<GraphID> {};

        const auto creates =
            task.creates() | std::views::filter([this](const auto resource_id) noexcept {
                const auto& resource = getResource(resource_id);
                return core::is<GraphImage>(resource);
            }) |
            std::views::transform([&, this](const auto id) noexcept {
                const auto& resource    = getResource<ImageDescription>(id);
                const auto& description = resource.description();

                auto attachment_description =
                    vk::AttachmentDescription { .format          = description.format,
                                                .loadOp          = vk::AttachmentLoadOp::eClear,
                                                .storeOp         = vk::AttachmentStoreOp::eStore,
                                                .stencilLoad_op  = vk::AttachmentLoadOp::eDontCare,
                                                .stencilStore_op = vk::AttachmentStoreOp::eDontCare,
                                                .sourceLayout    = vk::ImageLayout::eUndefined,
                                                .destinationLayout =
                                                    vk::ImageLayout::eColorAttachmentOptimal };

                if (isDepthStencilFormat(description.format)) [[unlikely]] {
                    std::swap(attachment_description.loadOp, attachment_description.stencilLoadOp);
                    std::swap(attachment_description.storeOp,
                              attachment_description.stencilStoreOp);
                    attachment_description.destinationLayout =
                        vk::ImageLayout::eDepthStencilAttachmentOptimal;
                }

                layouts[id] = attachment_description.destinationLayout;

                return attachment_description;
            }) |
            std::ranges::to<std::vector>();

        const auto writes =
            task.writes() | std::views::filter([this](const auto id) noexcept {
                const auto& resource = getResource(id);
                return core::is<GraphImage>(resource);
            }) |
            std::views::transform([&, this](const auto id) {
                const auto& resource    = getResource<ImageDescription>(id);
                const auto& description = resource.description();

                auto attachment_description =
                    vk::AttachmentDescription { .format         = description.format,
                                                .loadOp         = vk::AttachmentLoadOp::eClear,
                                                .storeOp        = vk::AttachmentStoreOp::eStore,
                                                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                                                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                                .sourceLayout   = layouts.at(id),
                                                .destinationLayout = layouts.at(id) };

                if (isDepthStencilFormat(description.format)) [[unlikely]] {
                    std::swap(attachment_description.loadOp, attachment_description.stencilLoadOp);
                    std::swap(attachment_description.storeOp,
                              attachment_description.stencilStoreOp);
                    attachment_description.destinationLayout =
                        vk::ImageLayout::eDepthStencilAttachmentOptimal;
                }

                layouts[id] = attachment_description.destinationLayout;

                return attachment_description;
            }) |
            std::ranges::to<std::vector>();

        const auto reads =
            task.reads() | std::views::filter([this](const auto id) noexcept {
                const auto& resource = getResource(id);
                return core::is<GraphImage>(resource);
            }) |
            std::views::transform([&, this](const auto id) {
                const auto& resource    = getResource<ImageDescription>(id);
                const auto& description = resource.description();

                auto attachment_description =
                    vk::AttachmentDescription { .format         = description.format,
                                                .loadOp         = vk::AttachmentLoadOp::eLoad,
                                                .storeOp        = vk::AttachmentStoreOp::eDontCare,
                                                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                                                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                                .initialLayout  = layouts.at(id),
                                                .finalLayout    = layouts.at(id) };

                if (std::ranges::any_of(writes, core::binaryToUnary(id, std::equal_to {}))) {
                    to_remove.emplace_back(id);
                    attachment_description.storeOp = vk::AttachmentStoreOp::eStore;
                }

                if (isDepthStencilFormat(description.format)) [[unlikely]] {
                    std::swap(attachment_description.loadOp, attachment_description.stencilLoadOp);
                    std::swap(attachment_description.storeOp,
                              attachment_description.stencilStoreOp);
                    attachment_description.finalLayout =
                        vk::ImageLayout::eDepthStencilAttachmentOptimal;
                }

                layouts[id] = attachment_description.finalLayout;

                return attachment_description;
            }) |
            std::ranges::to<std::vector>();

        auto output = VulkanRenderPassData {};
        output.attachment_descriptions =
            core::merge(std::move(creates), std::move(reads), std::move(writes));
        output.render_pass_create_info =
            vk::RenderPassCreateInfo {}.setAttachments(output.attachment_descriptions);

        auto& color_attachment_ref = output.color_attachment_refs.emplace_back();
        color_attachment_ref.reserve(std::size(output.attachment_descriptions));
        auto& depth_attachment_ref   = output.color_attachment_refs.emplace_back();
        auto& resolve_attachment_ref = output.resolve_attachment_refs.emplace_back();

        for (auto&& [i, attachment] : output.attachment_descriptions | std::views::enumerate) {
            auto attachment_ref = &color_attachment_ref;

            if (isDepthFormat(attachment.format)) attachment_ref = &depth_attachment_ref;

            attachment_ref->emplace_back(
                vk::AttachmentReference {} .setAttachment(i).setLayout(attachment.destinationLayout);
        }

        // TODO support multiple subpasses
        output.subpasses.emplace_back(
            vk::SubpassDescription {}
                .setPipelineBindPoint((task.type() == GraphTask::Type::Graphics)
                                          ? vk::PipelineBindPoint::eGraphics
                                          : vk::PipelineBindPoint::eCompute)
                .setColorAttachments(color_attachment_ref)
                .setDepthAttachments(depth_attachment_ref)
                .setResolveAttachments(resolve_attachment_ref));

        output.subpasses_deps.emplace_back(
            vk::SubpassDependency {}
                .setSrcSubpass(vk::SubpassExternal)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .dstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                               vk::AccessFlagBits::eColorAttachmentWrite));

        output.setSubpasses(output.subpass_descriptions);
        output.setDependencies(output.subpass_dependencies);

        return output;
    }

    auto FrameGraphBuilder::allocatePhysicalResources(const vk::raii::Device& device)
        -> BakedFrameGraph::Data {
        using Data = BakedFrameGraph::Data;

        auto output                = Data {};
        output.raster_command_pool = vk::raii::CommandPool {};
        output.main_cmb            = createCommandBuffer(device, output.raster_command_pool);
        output.blit_cmb            = createCommandBuffer(device, output.raster_command_pool);

        // auto future = core::parallelTransform(
        //     m_thread_pool,
        //     m_preprocessed_framegraph,
        //     [&](const auto& pass) {
        //         auto task = Data::Task {};
        //         task.id   = pass.id;
        //
        //         for (const auto& info : pass.buffers) {
        //             auto create_info = vk::BufferCreateInfo{}
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
        //             pass.name)); task.framebuffer = task.renderpass->allocateFramebuffer(extent,
        //             attachments); device.setObjectName(*task.framebuffer,
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
        //         const auto inheritance_info = vk::CommandBufferInheritanceInfo {
        //         }
        //         const auto secondary_begin_info =
        //             vk::CommandBufferBeginInfo {}
        //                 .setIneri
        //
        //                     task.commandbuffer->begin(false,
        //                                               vk::InheritanceInfo {
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
