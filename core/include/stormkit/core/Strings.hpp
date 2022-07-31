// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>

#include <functional>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/HashMap.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    using CZString  = const char *;
    using ZString   = char *;
    using CWZString = const wchar_t *;
    using WZString  = wchar_t *;

    auto split(std::string_view string, char delim) -> std::vector<std::string>;

    template<std::integral T, bool complete = true>
    auto intToHex(T i) -> std::string;

    auto toLower(std::string_view string, std::locale locale = std::locale("")) -> std::string;
    auto toUpper(std::string_view string, std::locale locale = std::locale("")) -> std::string;

    struct StringHash {
        using is_transparent = void;

        [[nodiscard]] auto operator()(const std::string &value) const noexcept -> core::Hash64;
        [[nodiscard]] auto operator()(std::string_view value) const noexcept -> core::Hash64;
    };

    struct StringEqual {
        using is_transparent = void;

        [[nodiscard]] auto operator()(const std::string &a, const std::string &b) const noexcept
            -> bool;
        [[nodiscard]] auto operator()(const std::string &a, std::string_view b) const noexcept
            -> bool;
        [[nodiscard]] auto operator()(std::string_view a, const std::string &b) const noexcept
            -> bool;
        [[nodiscard]] auto operator()(std::string_view a, std::string_view b) const noexcept
            -> bool;
    };

    template<class T>
    using StringHashMap = HashMap<std::string, T, StringHash, StringEqual>;
} // namespace stormkit::core

#include "Strings.inl"