// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef UTILS_HPP
#define UTILS_HPP

namespace stormkit::wsi::macos {
    auto mouseButton(int button) noexcept -> int;
    auto localizedKey(char code) noexcept -> int;
    auto nonLocalizedKey(unsigned short code) noexcept -> int;
    auto usageToVirtualCode(int usage) noexcept -> int;
} // namespace stormkit::wsi::macos

#endif
