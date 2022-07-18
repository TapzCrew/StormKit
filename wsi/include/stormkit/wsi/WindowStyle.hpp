// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FlagsMacro.hpp>

#undef DELETE

#include <stormkit/core/Flags.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::wsi {
    enum class WindowStyle : core::UInt8 {
        TitleBar    = 0b1,
        Close       = TitleBar | 0b10,
        Minimizable = TitleBar | 0b100,
        Resizable   = TitleBar | 0b1000,
        All         = Resizable | Minimizable | Close
    };
}

FLAG_ENUM(stormkit::wsi::WindowStyle)
