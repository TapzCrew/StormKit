// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::name() const noexcept -> const std::string & { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::setName(std::string name) noexcept -> void { m_name = std::move(name); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::parent() const noexcept -> TreeNode::IndexType { return m_parent; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::setParent(IndexType index) noexcept -> void { m_parent = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::nextSibling() const noexcept -> TreeNode::IndexType {
        return m_next_sibling;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::setNextSibling(IndexType index) noexcept -> void {
        m_next_sibling = index;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::firstChild() const noexcept -> TreeNode::IndexType {
        return m_first_child;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::setFirstChild(IndexType index) noexcept -> void { m_first_child = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::dirtyBits() const noexcept -> const TreeNode::DirtyBitType & {
        return m_dirty_bits;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto TreeNode::setDirtyBits(DirtyBitType bits) noexcept -> void { m_dirty_bits = bits; }
} // namespace stormkit::core
