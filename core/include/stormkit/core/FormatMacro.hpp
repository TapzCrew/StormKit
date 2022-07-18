// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <version>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 202110L
    #define STORMKIT_HAS_STL_FORMAT
#elif __has_include(<fmt/format.h>)
    #define STORMKIT_HAS_FMT_FORMAT
    #include <fmt/format.h>
#else
    #error "No Format fallback"
#endif

#ifdef STORMKIT_HAS_STL_FORMAT
    #define FORMATTER(TYPE, OUTPUT_STR, ...)                        \
        inline auto toString(const TYPE& data) noexcept {           \
            return stormkit::core::format(OUTPUT_STR, __VA_ARGS__); \
        }

    #define CUSTOM_FORMAT(TYPE, METHOD)                            \
        template<>                                                 \
        struct std::formatter<TYPE>: std::formatter<std::string> { \
            auto format(const TYPE& data, format_context& ctx) {   \
                std::ranges::copy(METHOD, ctx.out());              \
                return ctx.out();                                  \
            }                                                      \
        };
    #define CUSTOM_FORMAT_STRING_VIEW(TYPE, METHOD)                     \
        template<>                                                      \
        struct std::formatter<TYPE>: std::formatter<std::string_view> { \
            auto format(const TYPE& data, format_context& ctx) {        \
                std::ranges::copy(METHOD, ctx.out());                   \
                return ctx.out();                                       \
            }                                                           \
        };
#elif defined(STORMKIT_HAS_FMT_FORMAT)
    #define FORMATTER(TYPE, OUTPUT_STR, ...)                        \
        inline auto toString(const TYPE& data) noexcept {           \
            return stormkit::core::format(OUTPUT_STR, __VA_ARGS__); \
        }

    #define CUSTOM_FORMAT(TYPE, METHOD)                                                \
        template<>                                                                     \
        struct fmt::formatter<TYPE>: formatter<std::string> {                          \
            template<typename FormatContext>                                           \
            auto format(const TYPE& data, FormatContext& ctx) -> decltype(ctx.out()) { \
                auto foo = METHOD;                                                     \
                std::copy(std::begin(foo), std::end(foo), ctx.out());                  \
                return ctx.out();                                                      \
            }                                                                          \
        };

    #define CUSTOM_FORMAT_STRING_VIEW(TYPE, METHOD)                                    \
        template<>                                                                     \
        struct fmt::formatter<TYPE>: formatter<std::string_view> {                     \
            template<typename FormatContext>                                           \
            auto format(const TYPE& data, FormatContext& ctx) -> decltype(ctx.out()) { \
                auto foo = METHOD;                                                     \
                std::copy(std::begin(foo), std::end(foo), ctx.out());                  \
                return ctx.out();                                                      \
            }                                                                          \
        };
#endif
