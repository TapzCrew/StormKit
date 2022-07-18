// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <stormkit/core/Fwd.hpp>
#include <stormkit/core/TreeNode.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    template<typename TreeNodeClass = TreeNode>
    class Tree {
      public:
        constexpr static const USize DEFAULT_PREALLOCATED_TREE_SIZE = 1000;

        using TreeNodeType         = TreeNodeClass;
        using TreeNodeIndexType    = typename TreeNodeType::IndexType;
        using TreeNodeDirtyBitType = typename TreeNodeType::DirtyBitType;

        Tree();
        ~Tree();

        Tree(const Tree &);
        auto operator=(const Tree &) -> Tree &;

        Tree(Tree &&);
        auto operator=(Tree &&) -> Tree &;

        auto getFreeNode() -> TreeNodeIndexType;

        auto insert(TreeNodeType &&node,
                    TreeNodeIndexType parent_index,
                    TreeNodeIndexType previous_sibling) -> TreeNodeIndexType;
        auto remove(TreeNodeIndexType index) -> void;

        auto markDirty(TreeNodeIndexType index, TreeNodeDirtyBitType bits) -> void;

        auto operator[](TreeNodeIndexType index) noexcept -> TreeNodeType &;
        auto operator[](TreeNodeIndexType index) const noexcept -> const TreeNodeType &;

        [[nodiscard]] auto size() const noexcept -> USize;

        [[nodiscard]] auto begin() noexcept;
        [[nodiscard]] auto begin() const noexcept;
        [[nodiscard]] auto cbegin() const noexcept;

        [[nodiscard]] auto end() noexcept;
        [[nodiscard]] auto end() const noexcept;
        [[nodiscard]] auto cend() const noexcept;

        auto clearDirties() noexcept -> void;
        [[nodiscard]] auto dirties() const noexcept -> std::span<const TreeNodeIndexType>;

        auto genDotFile(std::filesystem::path filepath,
                        std::function<std::string_view(std::string_view)> colorize_node) const
            -> void;

        auto genDotFile(std::filesystem::path filepath,
                        core::UInt32 highlight,
                        std::function<std::string_view(std::string_view)> colorize_node) const
            -> void;

      private:
        TreeNodeIndexType m_first_free_index = 0;
        std::vector<TreeNodeType> m_tree;
        std::vector<TreeNodeIndexType> m_dirties;
    };

    DECLARE_PTR_AND_REF_TEMPLATE((Tree), (class T))
} // namespace stormkit::core

#include "Tree.inl"
