// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Profiler::currentEntryIndex() const noexcept -> core::UInt32 {
        if (m_current_entry == 0) return ENTRY_COUNT - 1u;

        return m_current_entry - 1u;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Profiler::entries() const noexcept -> core::span<const Profiler::Entry> {
        return m_entries;
    }
} // namespace storm::engine
