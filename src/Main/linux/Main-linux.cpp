// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;
import stormkit.Core;

#include <version>

extern auto userMain(std::span<const std::string_view>) -> int;

auto main(int argc, char** argv) -> int {
#if not(defined(__cpp_lib_stacktrace) and __cpp_lib_stacktrace >= 202011L)
    stormkit::core::backtraceInit(argv[0]);
#endif

    auto args = std::vector<std::string_view> {};

    for (auto i : stormkit::core::range(argc)) args.emplace_back(argv[i]);

    return userMain(args);
}
