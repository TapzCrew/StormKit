// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.details.logcolorizer;

// clang-format off
/////////// - StormKit::core - ///////////
import stormkit.core.types;
// clang-format on
#else
    /////////// - StormKit::core - ///////////
    #include <stormkit/core/Types.mpp>

    /////////// - StormKit::log - ///////////
    #include "../LogColorizer.mpp"
#endif

#include <windows.h>

namespace stormkit::log::details {
    [[maybe_unused]] constinit core::UInt8 KBLA = 0;
    [[maybe_unused]] constinit core::UInt8 KRED = 12;
    [[maybe_unused]] constinit core::UInt8 KGRN = 2;
    [[maybe_unused]] constinit core::UInt8 KYEL = 14;
    [[maybe_unused]] constinit core::UInt8 KBLU = 9;
    [[maybe_unused]] constinit core::UInt8 KMAG = 13;
    [[maybe_unused]] constinit core::UInt8 KCYN = 11;
    [[maybe_unused]] constinit core::UInt8 KWHT = 15;
    [[maybe_unused]] constinit core::UInt8 KGRS = 8;

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin(Severity severity, bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

        core::UInt8 background = KBLA;
        core::UInt8 text       = KWHT;
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

        SetConsoleTextAttribute(handle, (background << 4) + text);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyEnd(bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, (KBLA << 4) + KWHT);
    }
} // namespace stormkit::log::details
