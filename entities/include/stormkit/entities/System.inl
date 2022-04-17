// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace stormkit::entities {
    inline auto System::priority() const noexcept -> core::UInt32 { return m_priority; }
    inline auto System::componentsUsed() const noexcept -> const ComponentTypes & {
        return m_types;
    }
} // namespace stormkit::entities
