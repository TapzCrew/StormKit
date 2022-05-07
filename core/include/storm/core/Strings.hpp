// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <iomanip>
#include <locale>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include <storm/core/Span.hpp>

#include <storm/core/Assert.hpp>
#include <storm/core/TypeTraits.hpp>
#include <storm/core/Types.hpp>

#ifdef UTF8_CPP20
    #include <cuchar>
#endif

#define STRINGIFY(x) #x
#define CASE(x) \
    case x:     \
        return STRINGIFY(x);

#if !defined(__cpp_lib_char8_t)
using char8_t     = unsigned char;
using std::string = std::basic_string<char8_t>;
#endif

using u8ofstream = std::basic_ofstream<char8_t>;
using u8ifstream = std::basic_ifstream<char8_t>;

namespace storm::core {
    inline std::vector<std::string> split(std::string_view string, char delim) {
        auto output = std::vector<std::string> {};
        auto first  = ArraySize { 0u };

        while (first < string.size()) {
            const auto second = string.find_first_of(delim, first);

            if (first != second) output.emplace_back(string.substr(first, second - first));

            if (second == std::string_view::npos) break;

            first = second + 1;
        }

        return output;
    }

    template<typename T, bool complete = true>
    std::string intToHex(T i) {
        static_assert(std::is_integral<T>::value,
                      "Template argument 'T' must be a fundamental integer type (e.g. "
                      "int, short, etc..).");

        auto stream = std::stringstream {};

        if constexpr (complete)
            stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2u) << std::hex;
        else
            stream << std::hex;

        if constexpr (std::is_same_v<core::UInt8, T>) stream << static_cast<core::Int32>(i);
        else if constexpr (std::is_same_v<core::Int8, T>)
            stream << static_cast<core::Int32>(static_cast<uint8_t>(i));
        else
            stream << i;

        return stream.str();
    }

    template<typename String = std::string>
    inline auto toLower(String &&string, std::locale locale = std::locale("")) {
        using StringRawType = std::remove_reference_t<String>;

        auto &f = std::use_facet<std::ctype<typename StringRawType::value_type>>(locale);

        auto result = StringRawType { std::forward<String>(string) };
        f.tolower(&result[0], &result[0] + result.size());

        return result;
    }

    template<typename String = std::string>
    inline String toUpper(String &&string, std::locale locale = std::locale("")) {
        using StringRawType = std::remove_reference_t<String>;

        auto &f = std::use_facet<std::ctype<typename StringRawType::value_type>>(locale);

        auto result = StringRawType { std::forward<String>(string) };
        f.toupper(&result[0], &result[0] + result.size());

        return result;
    }

    inline auto toU8String(std::string_view str) -> std::u8string {
#ifdef UTF8_CPP20
        auto state   = std::mbstate_t {};
        auto out_str = std::string { MB_LEN_MAX };

        std::mbrtoc8(std::data(out_str), std::data(str), std::size(str), &state);

        out_str.shrink_to_fit();

        return out_str;
#else
        auto out_str = std::u8string { reinterpret_cast<const char8_t *>(std::data(str)) };

        return out_str;
#endif
    }

    inline auto toAsciiString(std::string_view str) -> std::string {
#ifdef UTF8_CPP20
        auto state          = std::mbstate_t {};
        std::string out_str = std::string { MB_LEN_MAX };

        for (const auto &c : str) std::c8rtomb(std::data(out_str), c, &state);

        out_str.shrink_to_fit();

        return out_str;
#else
        auto out_str = std::string { reinterpret_cast<const char *>(std::data(str)) };

        return out_str;
#endif
    }
} // namespace storm::core
