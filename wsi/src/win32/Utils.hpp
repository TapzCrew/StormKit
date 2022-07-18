// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <windows.h>

#include <stormkit/wsi/Key.hpp>

namespace stormkit::wsi::details::win32 {
    auto win32KeyToStormKitKey(WPARAM key, LPARAM flags) noexcept -> Key;
    auto stormKitKeyToWin32(Key key) noexcept -> int;
    auto win32KeyToChar(WPARAM key, LPARAM flags) noexcept -> char;
} // namespace stormkit::wsi::details::win32
