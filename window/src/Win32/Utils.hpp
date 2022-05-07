// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - Win32Api - ///////////
#include <Windows.h>

/////////// - StormKit::window - ///////////
#include <storm/window/Key.hpp>

namespace storm::window::details {
    STORMKIT_PRIVATE Key win32KeyToStormKitKey(WPARAM key, LPARAM flags) noexcept;
    STORMKIT_PRIVATE int stormKitKeyToWin32(Key key) noexcept;
    STORMKIT_PRIVATE char win32KeyToChar(WPARAM key, LPARAM flags) noexcept;
} // namespace storm::window::details
