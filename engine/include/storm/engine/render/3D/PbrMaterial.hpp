// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC PbrMaterial {
      public:
        PbrMaterial() noexcept;
        ~PbrMaterial();

        PbrMaterial(PbrMaterial &&) noexcept;
        PbrMaterial &operator=(PbrMaterial &&) noexcept;
    };
} // namespace storm::engine

#include "PbrMaterial.inl"
