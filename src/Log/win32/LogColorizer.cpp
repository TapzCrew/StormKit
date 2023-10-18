// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <windows.h>

module stormkit.Log;

import std;

import stormkit.Core;

import :LogColorizer;
import :Severity;

namespace stormkit::log::details {
    namespace {
        constexpr auto KBLA = 0;
        constexpr auto KRED = 12;
        constexpr auto KGRN = 2;
        constexpr auto KYEL = 14;
        constexpr auto KBLU = 9;
        constexpr auto KMAG = 13;
        constexpr auto KCYN = 11;
        constexpr auto KWHT = 15;
        constexpr auto KGRS = 8;
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyBegin(Severity severity, bool to_stderr) noexcept -> void {
        auto output = (to_stderr) ? core::getSTDErr() : core::getSTDOut();

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
        }

        SetConsoleTextAttribute(output, (background << 4) + text);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyEnd(bool to_stderr) noexcept -> void {
        auto output = (to_stderr) ? core::getSTDErr() : core::getSTDOut();
        SetConsoleTextAttribute(output, (KBLA << 4) + KWHT);
    }
} // namespace stormkit::log::details
