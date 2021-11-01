// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <ranges>

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Concepts.hpp>

/*#if __has_include(<format>) && !defined(STORMKIT_OS_MACOS) && !defined(STORMKIT_GEN_DOC)
    #include <format>
namespace storm::core {
    template<class ... Args>
    auto format(std::string_view f, const Args& ... args) -> std::string {
        return std::format(f, args...);
    }

    template<class OutputIt, class ... Args>
    auto format_to(OutputIt it, std::string_view f, const Args& ... args) -> OutputIt {
        return std::format_to(it, f, args...);
    }

} // namespace storm::core

    #define FORMATTER(TYPE, OUTPUT_STR, ...) \
    inline auto toString(const TYPE &data) noexcept { \
        return storm::core::format(OUTPUT_STR, __VA_ARGS__); \
    }

#define CUSTOM_FORMAT(TYPE, METHOD)                          \
        template<>                                                    \
        struct std::formatter<TYPE> : std::formatter<std::string> {   \
            auto format(const TYPE &data, format_context &ctx) {       \
                std::ranges::copy(METHOD, ctx.out()); \
return ctx.out(); \
            }                                                         \
        };
#define CUSTOM_FORMAT_STRING_VIEW(TYPE, METHOD)                          \
        template<>                                                    \
        struct std::formatter<TYPE> : std::formatter<std::string_view> {   \
            auto format(const TYPE &data, format_context &ctx) {       \
                std::ranges::copy(METHOD, ctx.out()); \
return ctx.out(); \
            }                                                         \
        };

template <storm::core::Enumeration Enum>
struct std::formatter<Enum> : std::formatter<std::underlying_type_t<Enum>> {
    auto format(Enum e, format_context &ctx) {
        using underlying_type = std::underlying_type_t<Enum>;

        return std::formatter<underlying_type>::format(static_cast<underlying_type>(e), ctx); \
    }
};
#elif __has_include(<fmt/format.h>)*/
    #include <fmt/format.h>
namespace storm::core {
    template<class ... Args>
    auto format(std::string_view f, const Args& ... args) -> std::string {
        return fmt::format(f, args...);
    }

    template<class OutputIt, class ... Args>
    auto format_to(OutputIt it, std::string_view f, const Args& ... args) -> OutputIt {
        return fmt::format_to(it, f, args...);
    }
} // namespace storm::core


    #define FORMATTER(TYPE, OUTPUT_STR, ...) \
    inline auto toString(const TYPE &data) noexcept { \
        return storm::core::format(OUTPUT_STR, __VA_ARGS__); \
    }
    #define CUSTOM_FORMAT(TYPE, METHOD)                          \
    namespace fmt {                                                   \
        template<>                                                    \
        struct formatter<TYPE> {                                      \
            template<typename ParseContext>                           \
            constexpr auto parse(ParseContext &ctx) {                 \
                return ctx.begin();                                   \
            }                                                         \
            template<typename FormatContext>                          \
            auto format(const TYPE &data, FormatContext &ctx) {       \
               auto foo = METHOD; \
               std::copy(std::begin(foo), std::end(foo), ctx.out()); \
               return ctx.out();  \
            }                                                         \
        };                                                            \
    }
    #define CUSTOM_FORMAT_STRING_VIEW(TYPE, METHOD)                          \
    namespace fmt {                                                   \
        template<>                                                    \
        struct formatter<TYPE> {                                      \
            template<typename ParseContext>                           \
            constexpr auto parse(ParseContext &ctx) {                 \
                return ctx.begin();                                   \
            }                                                         \
            template<typename FormatContext>                          \
            auto format(const TYPE &data, FormatContext &ctx) {       \
               auto foo = METHOD; \
               std::copy(std::begin(foo), std::end(foo), ctx.out()); \
               return ctx.out();  \
            }                                                         \
        };                                                            \
    }

//#endif
