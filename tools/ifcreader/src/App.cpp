// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <windows.h>

module App;

import std;

import stormkit.Core;

import IFC;

using namespace stormkit;

// hack as msvc have a bug with exporting stl partial specialision
template<>
struct std::formatter<Architecture>: formatter<string_view> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return formatter<string_view>::parse(ctx);
    };

    auto format(Architecture data, format_context& ctx) const -> decltype(ctx.out()) {
        return formatter<string_view>::format(toStringView(data), ctx);
    }
};

template<>
struct std::formatter<Parser::Error>: formatter<string_view> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return formatter<string_view>::parse(ctx);
    };

    auto format(Parser::Error data, format_context& ctx) const -> decltype(ctx.out()) {
        return formatter<string_view>::format(toStringView(data), ctx);
    }
};

template<>
struct std::formatter<UnitSort>: formatter<string_view> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return formatter<string_view>::parse(ctx);
    };

    auto format(UnitSort data, format_context& ctx) const -> decltype(ctx.out()) {
        return formatter<string_view>::format(toStringView(data), ctx);
    }
};

/////////////////////////////////////
/////////////////////////////////////
App::App() {
}

/////////////////////////////////////
/////////////////////////////////////
App::~App() = default;

/////////////////////////////////////
/////////////////////////////////////
auto App::run(const int argc, const char **argv) -> core::Int32 {
    using namespace std::string_literals;

    auto result = Parser::load(argv[1]);

    if (!result) {
        std::cerr << std::format("Failed to parse file {}, reason: {}", argv[1], result.error());

        return EXIT_FAILURE;
    }

    const auto ifc = std::move(result).value();
    core::vprintln("{}", sizeof(SHA256));
    core::println("┌{0:─^{1}}┐\n"
                  "│ src_path: {2: >{1}} │\n"
                  "│ major_version: {3: >33} │\n"
                  "│ minor_version: {4: >33} │\n"
                  "│ abi: {5: >43} │\n"
                  "│ arch: {6: >42} │\n"
                  "│ dialect: {7: >39} │\n"
                  "│ unit_sort: {8: >37} │\n"
                  "│ partition_count: {9: >31} │\n"
                  "│ internal: {10: >38} │\n"
                  "└{11:─^{1}}┘",
                  " Module ",
                  50,
                  ifc.src_path,
                  ifc.major_version,
                  ifc.minor_version,
                  ifc.abi,
                  ifc.arch,
                  ifc.dialect,
                  core::as<UnitSort>(ifc.unit.sort),
                  std::size(ifc.partitions),
                  ifc.internal,
                  "");

    core::println("┌{0:─^{1}}┐", "Partitions", 67);
    for (const auto& entry : ifc.partitions) {
        core::println("├────────── name: {:<49} │", entry.name);
        core::println("│{0: <67}│", "");
    }
    core::println("└{0:─^{1}}┘", "", 67);

    std::fflush(stdout);

    return EXIT_SUCCESS;
}
