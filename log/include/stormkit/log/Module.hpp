// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <string>

#include <stormkit/core/NamedType.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/log/Fwd.hpp>

namespace stormkit::log {
    struct Module {
        template<typename... Args>
        auto dlog(Args&&...args) const -> void;

        template<typename... Args>
        auto ilog(Args&&...args) const -> void;

        template<typename... Args>
        auto wlog(Args&&...args) const -> void;

        template<typename... Args>
        auto elog(Args&&...args) const -> void;

        template<typename... Args>
        auto flog(Args&&...args) const -> void;

        std::string name;
    };

    [[nodiscard]] auto operator"" _module(const char *name, core::USize sz)
        -> stormkit::log::Module;
} // namespace stormkit::log
