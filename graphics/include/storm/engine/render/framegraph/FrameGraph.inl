// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::setBackbuffer(FrameGraphResourceID id) -> void {
        m_graph.backbuffer_id = id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<details::FrameGraphDescriptorConcept Descriptor>
    auto FrameGraph::setRetainedResource(std::string name,
                                         Descriptor descriptor,
                                         typename Descriptor::PhysicalResource &resource)
        -> FrameGraphResourceID {
        auto id = createResource<Descriptor>(std::move(name), descriptor, false);

        if constexpr (std::is_same_v<typename Descriptor::PhysicalResource, render::Texture>) {
            const auto vec_position = std::size(m_graph.retained_textures);

            m_graph.retained_textures.emplace_back(std::ref(resource));

            m_graph.resource_link[id] = vec_position;
        } else {
            const auto vec_position = std::size(m_graph.retained_buffers);

            m_graph.retained_buffers.emplace_back(std::ref(resource));

            m_graph.resource_link[id] = vec_position;
        }

        return id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraph::addPassNode(
        std::string name,
        typename FrameGraphPassNode<PassData>::SetupCallback &&setup_callback,
        typename FrameGraphPassNode<PassData>::ExecuteCallback &&execute_callback,
        QueueFlag flags) -> FrameGraphPassNode<PassData> & {
        using PassNodeType        = FrameGraphPassNode<PassData>;
        using SetupCallbackType   = typename PassNodeType::SetupCallback;
        using ExecuteCallbackType = typename PassNodeType::ExecuteCallback;

        const auto id = m_graph.next_pass_id++;

        auto &frame_pass = *m_graph.nodes.emplace_back(
            PassNodeType::allocateOwned(std::move(name),
                                        id,
                                        flags,
                                        std::forward<SetupCallbackType>(setup_callback),
                                        std::forward<ExecuteCallbackType>(execute_callback)));

        auto builder = FrameGraphBuilder { *this, frame_pass };
        frame_pass.setup(builder);

        return static_cast<PassNodeType &>(frame_pass);
    }

    template<details::FrameGraphDescriptorConcept Descriptor>
    FrameGraphResourceID
        FrameGraph::createResource(std::string name, Descriptor descriptor, bool transient) {
        const auto id = m_graph.next_resource_id++;
        m_graph.resources.emplace_back(
            FrameGraphResource::allocateOwned(std::move(name), id, transient, descriptor));

        return id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<details::FrameGraphDescriptorConcept Descriptor>
    auto FrameGraph::createTransientResource(std::string name, Descriptor descriptor)
        -> FrameGraphResourceID {
        auto id = createResource<Descriptor>(std::move(name), descriptor, true);

        return id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::physicalTextures() noexcept -> std::span<PhysicalTexture> {
        return m_textures;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::physicalTextures() const noexcept -> std::span<const PhysicalTexture> {
        return m_textures;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::physicalBuffers() noexcept -> std::span<PhysicalBuffer> {
        return m_buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::physicalBuffers() const noexcept -> std::span<const PhysicalBuffer> {
        return m_buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::getPhysicalTexture(FrameGraphResourceID id) noexcept
        -> render::Texture & {
        auto &resource = getResource(id);
        if (resource.isTransient()) {
            auto it = core::ranges::find_if(m_textures,
                                            [&id](auto &texture) { return texture.id == id; });

            STORMKIT_ENSURES(it != core::ranges::end(m_textures));

            return *it->texture;
        }

        return m_graph.retained_textures[m_graph.resource_link.at(id)].get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::getPhysicalTexture(FrameGraphResourceID id) const noexcept
        -> const render::Texture & {
        auto &resource = getResource(id);
        if (resource.isTransient()) {
            auto it = core::ranges::find_if(m_textures, [&id](const auto &texture) {
                return texture.id == id;
            });

            STORMKIT_ENSURES(it != core::ranges::cend(m_textures));

            return *it->texture;
        }

        return m_graph.retained_textures[m_graph.resource_link.at(id)].get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::getPhysicalBuffer(FrameGraphResourceID id) noexcept
        -> render::HardwareBuffer & {
        auto &resource = getResource(id);
        if (resource.isTransient()) {
            auto it =
                core::ranges::find_if(m_buffers, [&id](auto &buffer) { return buffer.id == id; });

            STORMKIT_ENSURES(it != core::ranges::end(m_buffers));

            return *it->buffer;
        }

        return m_graph.retained_buffers[m_graph.resource_link.at(id)].get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraph::getPhysicalBuffer(FrameGraphResourceID id) const noexcept
        -> const render::HardwareBuffer & {
        auto &resource = getResource(id);
        if (resource.isTransient()) {
            auto it = core::ranges::find_if(m_buffers,
                                            [&id](const auto &buffer) { return buffer.id == id; });

            STORMKIT_ENSURES(it != core::ranges::cend(m_buffers));

            return *it->buffer;
        }

        return m_graph.retained_buffers[m_graph.resource_link.at(id)].get();
    }
} // namespace storm::engine
