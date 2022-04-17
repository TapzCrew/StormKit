// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <chrono>
#include <utility>

/////////// - StormKit::core - ///////////
#include <storm/core/Types.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Profiler {
      public:
        static constexpr auto ENTRY_COUNT = 100u;

        using Clock = std::chrono::high_resolution_clock;
        struct Scope {
            core::UInt32 level;

            Clock::time_point start;
            Clock::time_point end;

            bool finalized = false;
        };

        struct Entry {
            Clock::time_point start;
            Clock::time_point end;

            std::vector<std::pair<std::string, Scope>> scopes;
        };

        Profiler() noexcept;
        ~Profiler() noexcept;

        Profiler(const Profiler &) noexcept;
        Profiler &operator=(const Profiler &) noexcept;

        Profiler(Profiler &&) noexcept;
        Profiler &operator=(Profiler &&) noexcept;

        void newFrame();
        void beginStage(std::string name, core::UInt32 level = 1);
        void endStage(std::string_view name);

        [[nodiscard]] core::UInt32 currentEntryIndex() const noexcept;
        [[nodiscard]] core::span<const Entry> entries() const noexcept;

      private:
        std::array<Entry, ENTRY_COUNT> m_entries;

        core::UInt32 m_current_entry = 0u;
        bool m_first_iteration       = true;
    };
} // namespace storm::engine

#include "Profiler.inl"
