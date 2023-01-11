// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#define __SPECSTRINGS_STRICT_LEVEL 0
#include <windows.h>
#undef __nullnullterminated

#include <cstdio>
#include <cstdlib>

import std;

constexpr auto BUF_SIZE = 1024;

extern auto main(std::span<const std::string_view>) -> int;

auto WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int {
    auto argc  = __argc;
    auto& argv = __argv;

    std::locale::global(std::locale { "" });
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setvbuf(stderr,
            nullptr,
            _IOFBF,
            BUF_SIZE); // on Windows 10+ we need buffering or console will get 1
                       // byte at a time (screwing up utf-8 encoding)
    setvbuf(stdout, nullptr, _IOFBF, BUF_SIZE);

    auto args = std::vector<std::string_view> {};

    for (auto i : stormkit::core::range(argc)) args.emplace_back(argv[i]);

    return main(args);
}
