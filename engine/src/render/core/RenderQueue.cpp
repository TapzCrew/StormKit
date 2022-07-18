// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "stormkit/core/ThreadPool.hpp"
#include "stormkit/gpu/resource/Buffer.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <mutex>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>
#include <stormkit/engine/render/core/RenderQueue.hpp>
#include <stormkit/log/Logger.hpp>
#include <stormkit/log/LogMacro.hpp>

#include <stormkit/core/Numerics.hpp>

#include <stormkit/gpu/core/Device.hpp>

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/render/Renderer.hpp>

#include <stormkit/engine/render/MeshComponent.hpp>
#include <utility>

using namespace stormkit::core::literals;
using namespace std::literals;

#ifdef STORMKIT_RENDERER_MULTITHREADED
    #define LOCK(mutex) auto lock = std::unique_lock { mutex };
#else
    #define LOCK(mutex)
#endif

namespace stormkit::engine {
    NAMED_LOGGER(renderqueue_logger, "StormKit.Renderer.RenderQueue");

    static constexpr auto VERTEX_CREATE_INFO =
        gpu::Buffer::CreateInfo { .usages = gpu::BufferUsageFlag::Vertex |
                                            gpu::BufferUsageFlag::Transfert_Dst,
                                  .size     = 8_mib,
                                  .property = gpu::MemoryPropertyFlag::Device_Local };
    static constexpr auto INDEX_CREATE_INFO =
        gpu::Buffer::CreateInfo { .usages = gpu::BufferUsageFlag::Index |
                                            gpu::BufferUsageFlag::Transfert_Dst,
                                  .size     = 8_mib,
                                  .property = gpu::MemoryPropertyFlag::Device_Local };

    static constexpr auto POSITION_HANDLE =
        RenderQueue::StaticMeshInputBuffer::ComponentHandle { 0 };
    static constexpr auto NORMALS_HANDLE =
        RenderQueue::StaticMeshInputBuffer::ComponentHandle { 1 };
    static constexpr auto COLORS_HANDLE = RenderQueue::StaticMeshInputBuffer::ComponentHandle { 2 };
    static constexpr auto UVS_HANDLE    = RenderQueue::StaticMeshInputBuffer::ComponentHandle { 3 };
    static constexpr auto INDICES_HANDLE =
        RenderQueue::StaticMeshInputBuffer::ComponentHandle { 4 };

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::RenderQueue(Engine &engine)
        : EngineObject { engine }, m_shader_input { engine,
                                                    VERTEX_CREATE_INFO,
                                                    VERTEX_CREATE_INFO,
                                                    VERTEX_CREATE_INFO,
                                                    VERTEX_CREATE_INFO,
                                                    INDEX_CREATE_INFO } {}

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::~RenderQueue() {}

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::RenderQueue(RenderQueue &&other) noexcept
        : RenderQueue(std::move(other), std::lock_guard { other.m_mutex }) {}

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::RenderQueue(RenderQueue &&other, std::lock_guard<std::mutex> lock) noexcept
        : EngineObject { std::move(other) }, m_next_id { std::exchange(other.m_next_id, 0) },
          m_entries { std::move(other.m_entries) },
          m_to_be_added { std::move(other.m_to_be_added) }, m_to_be_removed { std::move(
                                                                other.m_to_be_removed) },
          m_shader_input { std::move(other.m_shader_input) }, m_command_buffers { std::move(
                                                                  other.m_command_buffers) },
          m_vertex_buffer_fences { std::move(other.m_vertex_buffer_fences) },
          m_vertex_staging_buffers { std::move(other.m_vertex_staging_buffers) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderQueue::operator=(RenderQueue &&other) noexcept -> RenderQueue & {
        if (&other == this) [[unlikely]]
            return *this;

        auto lock       = std::unique_lock { m_mutex, std::defer_lock };
        auto other_lock = std::unique_lock { other.m_mutex, std::defer_lock };

        std::lock(lock, other_lock);

        EngineObject::operator   =(std::move(other));
        m_next_id                = std::exchange(other.m_next_id, 0);
        m_entries                = std::move(other.m_entries);
        m_to_be_added            = std::move(other.m_to_be_added);
        m_to_be_removed          = std::move(other.m_to_be_removed);
        m_shader_input           = std::move(other.m_shader_input);
        m_command_buffers        = std::move(other.m_command_buffers);
        m_vertex_buffer_fences   = std::move(other.m_vertex_buffer_fences);
        m_vertex_staging_buffers = std::move(other.m_vertex_staging_buffers);

        return *this;
    }

    auto RenderQueue::addMesh(const MeshComponent &mesh) -> ID {
        auto lock = std::lock_guard { m_mutex };

        m_to_be_added.emplace_back(m_next_id, mesh);

        return m_next_id++;
    }

    auto RenderQueue::removeMesh(ID id) -> void {
        STORMKIT_EXPECTS(id != INVALID_ID);

#ifdef STORMKIT_RENDERER_MULTITHREADED
        auto lock = std::lock_guard { m_mutex };
#endif

        const auto it =
            std::ranges::any_of(m_entries, [&](const auto &entry) { return entry.id == id; });

        if (!it) {
            renderqueue_logger.elog("Trying to remove unknown mesh (ID: {})", id);

            return;
        }

        m_to_be_removed.push_back(id);
    }

    auto RenderQueue::update() -> void {
        const auto &device = engine().renderer().device();

#ifdef STORMKIT_RENDERER_MULTITHREADED
        auto lock = std::lock_guard { m_mutex };

        auto &thread_pool = m_engine->threadPool();
#endif

        for (const auto id : m_to_be_removed)
            const auto _ = std::ranges::remove_if(m_entries, [&](auto &entry) {
                if (entry.id == id) {
                    entry.id   = INVALID_ID;
                    entry.mesh = nullptr;

                    for (auto &submesh : entry.submeshes) {
                        if (submesh.positions.block != BlockBuffer::Block::Handle::invalidHandle())
                            [[likely]]
                            m_shader_input.freeBlock(submesh.positions);
                        if (submesh.normals.block != BlockBuffer::Block::Handle::invalidHandle())
                            [[likely]]
                            m_shader_input.freeBlock(submesh.normals);
                        if (submesh.colors.block != BlockBuffer::Block::Handle::invalidHandle())
                            [[likely]]
                            m_shader_input.freeBlock(submesh.colors);
                        if (submesh.uvs.block != BlockBuffer::Block::Handle::invalidHandle())
                            [[likely]]
                            m_shader_input.freeBlock(submesh.uvs);

                        if (submesh.indices.block != BlockBuffer::Block::Handle::invalidHandle())
                            m_shader_input.freeBlock(submesh.indices);
                    }

                    return true;
                }

                return false;
            });

        m_shader_input.defragment();

#ifdef STORMKIT_RENDERER_MULTITHREADED
        for (auto &future : futures) futures.wait();
#endif

        const auto &queue = (device.hasAsyncTransfertQueue()) ? device.asyncTransfertQueue()
                                                              : device.graphicsQueue();

        m_command_buffers = queue.createCommandBuffers(std::size(m_to_be_added));

        auto i = 0;
        for (auto &entry : m_to_be_added) {
            auto &cmb = m_command_buffers[i++]; // TODO benchmark one big command
                                                // buffer vs multiple command buffer
            device.setObjectName(cmb, "StormKit:CopyVertexData"sv);

            cmb.begin(true);
            cmb.beginDebugRegion("Copy vertex data");

            entry.submeshes = core::transform(
                entry.mesh->submeshes,
                [&](const auto &submesh) -> RenderEntry::SubMesh {
                    auto output          = RenderEntry::SubMesh {};
                    output.submesh_index = i++;

                    const auto positions = core::toConstByteSpan(submesh.vertices.positions());
                    const auto normals   = core::toConstByteSpan(submesh.vertices.normals());
                    const auto colors    = core::toConstByteSpan(submesh.vertices.colors());
                    const auto uvs       = core::toConstByteSpan(submesh.vertices.uvs());
                    const auto indices   = core::toConstByteSpan(submesh.indices);

                    output.positions =
                        m_shader_input.allocateBlock(std::size(positions), POSITION_HANDLE);
                    output.normals =
                        m_shader_input.allocateBlock(std::size(normals), NORMALS_HANDLE);
                    output.colors = m_shader_input.allocateBlock(std::size(colors), COLORS_HANDLE);
                    output.uvs    = m_shader_input.allocateBlock(std::size(uvs), UVS_HANDLE);

                    if (!std::empty(indices))
                        output.indices =
                            m_shader_input.allocateBlock(std::size(indices), INDICES_HANDLE);

                    const auto staging_size = std::size(positions) + std::size(normals) +
                                              std::size(colors) + std::size(uvs) +
                                              std::size(indices);

                    auto &staging_buffer = m_vertex_staging_buffers.emplace_back(
                        device.createStagingBuffer(staging_size));
                    auto staging_data = staging_buffer.map(0u, staging_size);

                    std::ranges::copy(positions, std::begin(staging_data));
                    std::ranges::copy(normals, std::begin(staging_data) + std::size(positions));
                    std::ranges::copy(colors,
                                      std::begin(staging_data) + std::size(positions) +
                                          std::size(normals));
                    std::ranges::copy(uvs,
                                      std::begin(staging_data) + std::size(positions) +
                                          std::size(normals) + std::size(colors));

                    if (!std::empty(indices))
                        std::ranges::copy(indices,
                                          std::begin(staging_data) + std::size(positions) +
                                              std::size(normals) + std::size(colors));

                    staging_buffer.flush(0, staging_size);
                    staging_buffer.unmap();

                    const auto &positions_block = m_shader_input.getBlock(output.positions);
                    const auto &normals_block   = m_shader_input.getBlock(output.normals);
                    const auto &colors_block    = m_shader_input.getBlock(output.colors);
                    const auto &uvs_block       = m_shader_input.getBlock(output.uvs);

                    const auto &positions_buffer = m_shader_input.getBuffer(output.positions);
                    const auto &normals_buffer   = m_shader_input.getBuffer(output.normals);
                    const auto &colors_buffer    = m_shader_input.getBuffer(output.colors);
                    const auto &uvs_buffer       = m_shader_input.getBuffer(output.uvs);

                    cmb.copyBuffer(staging_buffer,
                                   positions_buffer,
                                   positions_block.size,
                                   0,
                                   positions_block.offset);
                    cmb.copyBuffer(staging_buffer,
                                   normals_buffer,
                                   normals_block.size,
                                   positions_block.size,
                                   normals_block.offset);
                    cmb.copyBuffer(staging_buffer,
                                   colors_buffer,
                                   colors_block.size,
                                   positions_block.size + normals_block.size,
                                   colors_block.offset);
                    cmb.copyBuffer(staging_buffer,
                                   uvs_buffer,
                                   uvs_block.size,
                                   positions_block.size + normals_block.size + colors_block.size,
                                   uvs_block.offset);

                    if (!std::empty(indices)) {
                        const auto &indices_block  = m_shader_input.getBlock(output.indices);
                        const auto &indices_buffer = m_shader_input.getBuffer(output.indices);

                        cmb.copyBuffer(staging_buffer,
                                       indices_buffer,
                                       indices_block.size,
                                       positions_block.size + normals_block.size +
                                           colors_block.size + uvs_block.size,
                                       indices_block.offset);
                    }

                    return output;
                });

            cmb.endDebugRegion();
            cmb.end();

            auto &fence = m_vertex_buffer_fences.emplace_back(device.createFence());
            cmb.submit({}, {}, &fence);
        }

        auto first_invalid_entry = std::ranges::find_if(m_entries, [](const auto &entry) {
            return entry.id == INVALID_ID;
        });

        if (first_invalid_entry == std::ranges::cend(m_entries)) {
            m_entries.reserve(std::size(m_to_be_added) + std::size(m_entries));
            std::ranges::copy(m_to_be_added, std::back_inserter(m_entries));
        } else {
            const auto diff = std::distance(first_invalid_entry, std::ranges::end(m_entries));

            m_entries.reserve((std::size(m_to_be_added) + std::size(m_entries)) - diff);
            std::ranges::copy_n(std::ranges::cbegin(m_to_be_added), diff, first_invalid_entry);
            std::ranges::copy(std::ranges::cbegin(m_to_be_added) + diff,
                              std::ranges::cend(m_to_be_added),
                              std::back_inserter(m_entries));
        }

        m_to_be_added.clear();
        m_to_be_removed.clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderQueue::draw(gpu::CommandBuffer &cmb) -> void {
        if (!std::empty(m_vertex_buffer_fences)) {
            for (auto &fence : m_vertex_buffer_fences) fence.wait();

            m_command_buffers.clear();
            m_vertex_staging_buffers.clear();
            m_vertex_buffer_fences.clear();
        }

        for (const auto &entry : m_entries) {
            for (const auto &submesh : entry.submeshes) {
                const auto &_submesh = entry.mesh->submeshes[submesh.submesh_index];

                const auto &position_block = m_shader_input.getBlock(submesh.positions);
                const auto &normal_block   = m_shader_input.getBlock(submesh.normals);
                const auto &color_block    = m_shader_input.getBlock(submesh.colors);
                const auto &uv_block       = m_shader_input.getBlock(submesh.uvs);

                auto buffers = core::makeStaticArray(&m_shader_input.getBuffer(submesh.positions),
                                                     &m_shader_input.getBuffer(submesh.normals),
                                                     &m_shader_input.getBuffer(submesh.colors),
                                                     &m_shader_input.getBuffer(submesh.uvs));
                auto offsets = core::makeStaticArray(position_block.offset,
                                                     normal_block.offset,
                                                     color_block.offset,
                                                     uv_block.offset);

                cmb.bindVertexBuffers(buffers, offsets);

                if (submesh.indices.block == BlockBuffer::Block::Handle::invalidHandle()) {
                    const auto &buffer = m_shader_input.getBuffer(submesh.indices);
                    const auto &block  = m_shader_input.getBlock(submesh.indices);

                    cmb.bindIndexBuffer(buffer, block.offset, true);
                    cmb.drawIndexed(core::as<core::UInt32>(std::size(_submesh.indices)));
                } else
                    cmb.draw(core::as<core::UInt32>(std::size(_submesh.vertices.positions())));
            }
        }
    }
} // namespace stormkit::engine
