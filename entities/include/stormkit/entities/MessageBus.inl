// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto MessageBus::empty() const noexcept -> bool { return std::empty(m_messages); }
} // namespace stormkit::entities
