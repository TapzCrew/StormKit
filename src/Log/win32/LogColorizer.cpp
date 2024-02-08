// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <windows.h>

module stormkit.Log;

import std;

import stormkit.Core;

namespace stormkit::log::details {
    namespace {
        [[maybe_unused]] constexpr auto KBLA = 0;
        [[maybe_unused]] constexpr auto KRED = 12;
        [[maybe_unused]] constexpr auto KGRN = 2;
        [[maybe_unused]] constexpr auto KYEL = 14;
        [[maybe_unused]] constexpr auto KBLU = 9;
        [[maybe_unused]] constexpr auto KMAG = 13;
        [[maybe_unused]] constexpr auto KCYN = 11;
        [[maybe_unused]] constexpr auto KWHT = 15;
        [[maybe_unused]] constexpr auto KGRS = 8;
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyBegin(Severity severity, bool to_stderr) noexcept -> void {
        auto output =
            (to_stderr) ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);

        auto background = KBLA;
        auto text       = KWHT;
        switch (severity) {
            case Severity::Info:
                background = KGRN;
                text       = KBLA;
                break;
            case Severity::Warning:
                background = KMAG;
                text       = KBLA;
                break;
            case Severity::Error:
                background = KYEL;
                text       = KBLA;
                break;
            case Severity::Fatal:
                background = KRED;
                text       = KBLA;
                break;
            case Severity::Debug:
                background = KCYN;
                text       = KBLA;
                break;
            default: std::unreachable();
        }

        SetConsoleTextAttribute(output, (background << 4) + text);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyEnd(bool to_stderr) noexcept -> void {
        auto output =
            (to_stderr) ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(output, (KBLA << 4) + KWHT);
    }
} // namespace stormkit::log::details
