// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Module::dlog(Args&&...args) const -> void {
        Logger::dlog(*this, std::forward<Args>(args)...);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Module::ilog(Args&&...args) const -> void {
        Logger::ilog(*this, std::forward<Args>(args)...);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Module::wlog(Args&&...args) const -> void {
        Logger::wlog(*this, std::forward<Args>(args)...);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Module::elog(Args&&...args) const -> void {
        Logger::elog(*this, std::forward<Args>(args)...);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Module::flog(Args&&...args) const -> void {
        Logger::flog(*this, std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto operator"" _module(const char *str, [[maybe_unused]] core::USize sz)
        -> stormkit::log::Module {
        return Module { std::string { str, sz } };
    }
} // namespace stormkit::log
