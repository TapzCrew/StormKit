// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Flags.hpp>
#include <storm/core/Types.hpp>

namespace storm::window {
    enum class WindowStyle : core::UInt8 {
        TitleBar    = 0b1,
        Close       = TitleBar | 0b10,
        Minimizable = TitleBar | 0b100,
        Resizable   = TitleBar | 0b1000,
        All         = Resizable | Minimizable | Close
    };
}

FLAG_ENUM(storm::window::WindowStyle)
