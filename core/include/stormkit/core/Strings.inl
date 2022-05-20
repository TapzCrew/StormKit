// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto split(std::string_view string, char delim) -> std::vector<std::string> {
        auto output = std::vector<std::string> {};
        auto first  = USize { 0u };

        while (first < string.size()) {
            const auto second = string.find_first_of(delim, first);

            if (first != second) output.emplace_back(string.substr(first, second - first));

            if (second == std::string_view::npos) break;

            first = second + 1;
        }

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::integral T, bool complete>
    auto intToHex(T i) -> std::string {
        auto stream = std::stringstream {};

        if constexpr (complete)
            stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2u) << std::hex;
        else
            stream << std::hex;

        if constexpr (Is<UInt8, T>) stream << as<Int32>(i);
        else if constexpr (Is<Int8, T>)
            stream << as<Int32>(as<uint8_t>(i));
        else
            stream << i;

        return stream.str();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto toLower(std::string_view string, std::locale locale) -> std::string {
        auto &facet = std::use_facet<std::ctype<typename std::string_view::value_type>>(locale);

        auto result = std::string { string };
        facet.tolower(&result[0], &result[0] + result.size());

        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto toUpper(std::string_view string, std::locale locale) -> std::string {
        auto &facet = std::use_facet<std::ctype<typename std::string_view::value_type>>(locale);

        auto result = std::string { string };
        facet.toupper(&result[0], &result[0] + result.size());

        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringEqual::operator()(const std::string &a, const std::string &b) const noexcept
        -> bool {
        return a.compare(b);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringEqual::operator()(const std::string &a, std::string_view b) const noexcept
        -> bool {
        return a.compare(b);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringEqual::operator()(std::string_view a, const std::string &b) const noexcept
        -> bool {
        return a.compare(b);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringEqual::operator()(std::string_view a, std::string_view b) const noexcept
        -> bool {
        return a.compare(b);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringHash::operator()(const std::string &value) const noexcept -> core::Hash64 {
        return std::hash<std::string> {}(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StringHash::operator()(std::string_view value) const noexcept -> core::Hash64 {
        return std::hash<std::string_view> {}(value);
    }
} // namespace stormkit::core
