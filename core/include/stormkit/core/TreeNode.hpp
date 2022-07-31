// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <limits>
#include <string>

#include <stormkit/core/Fwd.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    class STORMKIT_PUBLIC TreeNode {
      public:
        using IndexType    = core::Handle32<TreeNode>;
        using DirtyBitType = core::UInt32;

        static constexpr auto INVALID_INDEX = std::numeric_limits<IndexType>::max();

        [[nodiscard]] auto name() const noexcept -> const std::string &;
        auto setName(std::string name) noexcept -> void;

        [[nodiscard]] auto parent() const noexcept -> IndexType;
        auto setParent(IndexType index) noexcept -> void;

        auto nextSibling() const noexcept -> IndexType;
        auto setNextSibling(IndexType index) noexcept -> void;

        auto firstChild() const noexcept -> IndexType;
        auto setFirstChild(IndexType index) noexcept -> void;

        auto dirtyBits() const noexcept -> const DirtyBitType &;
        auto setDirtyBits(DirtyBitType bits) noexcept -> void;

        auto invalidate() noexcept -> void;

      private:
        IndexType m_parent        = INVALID_INDEX;
        IndexType m_next_sibling  = INVALID_INDEX;
        IndexType m_first_child   = INVALID_INDEX;
        DirtyBitType m_dirty_bits = 0;

        std::string m_name;
    };
} // namespace stormkit::core

#include "TreeNode.inl"