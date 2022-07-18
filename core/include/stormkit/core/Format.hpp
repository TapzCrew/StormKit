// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <version>

// #if defined(__cpp_lib_format)
#if defined(__cpp_lib_format) && __cpp_lib_format >= 202110L
    #define STORMKIT_HAS_STL_FORMAT
#elif __has_include(<fmt/format.h>)
    #define STORMKIT_HAS_FMT_FORMAT
    #include <fmt/format.h>
#else
    #error "No Format fallback"
#endif

#include <ranges>
#include <string>
#include <vector>

#ifdef STORMKIT_HAS_STL_FORMAT
    #include <format>
#endif

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Concepts.hpp>

#if defined(STORMKIT_HAS_STL_FORMAT)
namespace stormkit::core {
    using std::format;
    using std::format_to;
    using std::make_format_args;
    using std::vformat;
    using std::vformat_to;

    constexpr auto runtime(std::string_view str) {
        return str;
    }
} // namespace stormkit::core

template<stormkit::core::EnumerationType Enum>
struct std::formatter<Enum>: std::formatter<std::underlying_type_t<Enum>> {
    auto format(Enum e, format_context& ctx) {
        using UnderlyingType = std::underlying_type_t<Enum>;

        return std::formatter<UnderlyingType>::format(stormkit::core::as<UnderlyingType>(e), ctx);
    }
};

template<std::ranges::range R>
    requires(!stormkit::core::StringLikeType<R>)
struct std::formatter<R>: std::formatter<typename R::value_type> {
    auto format(const R& range, format_context& ctx) {
        auto out = ctx.out();

        format_to(out, "[\n");

        if (!std::empty(range)) format_to(out, "    {}", range[0]);

        for (auto i = 1; i < std::ranges::size(range); ++i) format_to(out, ",\n    {}", range[i]);

        return format_to(out, "\n]");
    }
};
#elif defined(STORMKIT_HAS_FMT_FORMAT)
namespace stormkit::core {
    using fmt::format;
    using fmt::format_to;
    using fmt::make_format_args;
    using fmt::runtime;
    using fmt::vformat;
    using fmt::vformat_to;
} // namespace stormkit::core

template<class T>
struct fmt::formatter<std::vector<T>>: formatter<std::string> {
    template<typename FormatContext>
    auto format(const std::vector<T>& data, FormatContext& ctx) -> decltype(ctx.out()) {
        auto data_str = std::string {};
        for (const auto& d : data) data_str += stormkit::core::format("    {},\n", d);

        if (!std::empty(data_str)) {
            data_str.erase(data_str.find_last_of(','), 1);
            data_str.insert(std::begin(data_str), '\n');
        }

        auto output = stormkit::core::format("Array {{{}}}", data_str);
        std::copy(std::begin(output), std::end(output), ctx.out());

        return ctx.out();
    }
};

#endif
