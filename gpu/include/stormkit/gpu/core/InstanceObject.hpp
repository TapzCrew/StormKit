// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/gpu/core/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC InstanceObject {
      public:
        explicit InstanceObject(const Instance &instance) noexcept;
        ~InstanceObject() noexcept;

        InstanceObject(const InstanceObject &) noexcept;
        auto operator=(const InstanceObject &) noexcept -> InstanceObject &;

        InstanceObject(InstanceObject &&) noexcept;
        auto operator=(InstanceObject &&) noexcept -> InstanceObject &;

        [[nodiscard]] auto instance() const noexcept -> const Instance &;

      private:
        const Instance *m_instance;
    };
} // namespace stormkit::gpu

#include "InstanceObject.inl"
