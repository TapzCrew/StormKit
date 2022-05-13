// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/core/RenderQueue.mpp>
#include <stormkit/log/LogHandler.mpp>
#include <stormkit/log/LogMacro.hpp>

namespace stormkit::engine {
    LOGGER("StormKit.Renderer.RenderQueue");

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::RenderQueue() {}

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::~RenderQueue() {}

    /////////////////////////////////////
    /////////////////////////////////////
    RenderQueue::RenderQueue(RenderQueue &&other) noexcept {
        auto lock = std::lock_guard { other.m_mutex };

        m_next_id       = std::exchange(other.m_next_id, 0);
        m_entries       = std::move(other.m_entries);
        m_to_be_added   = std::move(other.m_to_be_added);
        m_to_be_removed = std::move(other.m_to_be_removed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderQueue::operator=(RenderQueue &&other) noexcept -> RenderQueue & {
        if (&other == this) [[unlikely]]
            return *this;

        auto lock       = std::unique_lock { m_mutex, std::defer_lock };
        auto other_lock = std::unique_lock { other.m_mutex, std::defer_lock };

        std::lock(lock, other_lock);

        m_next_id       = std::exchange(other.m_next_id, 0);
        m_entries       = std::move(other.m_entries);
        m_to_be_added   = std::move(other.m_to_be_added);
        m_to_be_removed = std::move(other.m_to_be_removed);

        return *this;
    }

    auto RenderQueue::addMesh(const MeshComponent &mesh) -> ID {
        auto lock = std::lock_guard { m_mutex };

        m_to_be_added.emplace_back(m_next_id, mesh);

        return m_next_id++;
    }

    auto RenderQueue::removeMesh(ID id) -> void {
        STORMKIT_EXPECTS(id != INVALID_ID);

        auto lock = std::lock_guard { m_mutex };

        const auto it =
            std::ranges::any_of(m_entries, [&](const auto &entry) { return entry.id == id; });

        if (!it) {
            elog("Trying to remove unknown mesh (ID: {})", id);

            return;
        }

        m_to_be_removed.push_back(id);
    }

    auto RenderQueue::update() -> void {
        auto lock = std::lock_guard { m_mutex };

        for (const auto id : m_to_be_removed)
            const auto _ = std::ranges::remove_if(m_entries, [id](auto &entry) {
                if (entry.id == id) {
                    entry.id   = INVALID_ID;
                    entry.mesh = nullptr;
                    return true;
                }

                return false;
            });

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
} // namespace stormkit::engine
