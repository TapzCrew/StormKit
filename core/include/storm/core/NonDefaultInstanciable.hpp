// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>

namespace storm::core {
    class STORMKIT_EBCO STORMKIT_PUBLIC NonDefaultInstanciable: public NonCopyable,
                                                                public NonMovable {
      public:
        NonDefaultInstanciable() = delete;

        ~NonDefaultInstanciable() noexcept = default;
    };
} // namespace storm::core
