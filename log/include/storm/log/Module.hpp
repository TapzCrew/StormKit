// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - GSL - ///////////
#include <gsl/string_span>

/////////// - StormKit::core - ///////////
#include <storm/core/NamedType.hpp>

namespace storm::log {
    using Module = core::NamedType<gsl::basic_zstring<const char>, struct LogModuleParameter>;

    constexpr storm::log::Module makeModule(gsl::basic_zstring<const char> str) {
        return core::makeNamed<Module>(str);
    }

    constexpr storm::log::Module operator"" _module(const char *str,
                                                    [[maybe_unused]] core::ArraySize sz) {
        return makeModule(str);
    }
} // namespace storm::log
