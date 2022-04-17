// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto makeModule(std::string_view str) -> stormkit::log::Module {
        return core::makeNamed<Module>(std::string { str });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto operator"" _module(const char *str, [[maybe_unused]] core::USize sz)
        -> stormkit::log::Module {
        return makeModule(std::string { str, sz });
    }
} // namespace stormkit::log
