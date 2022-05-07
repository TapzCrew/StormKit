// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Material: public core::NonCopyable {
      public:
        Material() noexcept;
        ~Material();

        Material(Material &&) noexcept;
        Material &operator=(Material &&) noexcept;

      private:
        std::vector<std::byte> m_data;
    };
} // namespace storm::engine

#include "Material.inl"
