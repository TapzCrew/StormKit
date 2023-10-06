// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import stormkit.Core;

import std;

import Test;

auto test_suites = std::vector<std::unique_ptr<test::TestSuiteHolder>> {};
auto verbose     = false;
auto failed      = false;

auto main(int argc, char **argv) noexcept -> int {
    for (auto i = 0; i < argc; ++i) {
        auto arg = std::string_view { argv[i] };
        if (arg == "--verbose" or arg == "-v") verbose = true;
    }

    auto return_code = 0;
    for (auto&& suite : test_suites) {
        std::println("Running test suite {} ({} tests)", suite->name, std::size(suite->tests));
        if (not suite->runTests()) return_code = -1;
    }

    return return_code;
}
