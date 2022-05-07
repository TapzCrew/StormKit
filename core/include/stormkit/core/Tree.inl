#pragma once

#include "Tree.mpp"

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    Tree<TreeNodeClass>::Tree() {
        m_tree.resize(DEFAULT_PREALLOCATED_TREE_SIZE);

        for (auto i = TreeNodeIndexType { 0u }; i < (std::size(m_tree) - 1u); ++i)
            m_tree[i].setNextSibling(i + 1u);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    Tree<TreeNodeClass>::~Tree() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    Tree<TreeNodeClass>::Tree(const Tree &) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    Tree<TreeNodeClass>::Tree(Tree &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::operator=(const Tree &) -> Tree & = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::operator=(Tree &&) -> Tree & = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::getFreeNode() -> TreeNodeIndexType {
        if (m_tree[m_first_free_index].nextSibling() == TreeNode::INVALID_INDEX) {
            const auto size      = as<float>(std::size(m_tree));
            const auto first_new = as<TreeNodeIndexType>(size);

            m_tree.resize(as<USize>(size * 1.5f));
            const auto new_size = std::size(m_tree);

            // generate a new chain of free objects, with the last one pointing to
            // ~0
            m_tree[m_first_free_index].setNextSibling(first_new);
            for (auto i = first_new; i < (new_size - 1u); ++i) m_tree[i].setNextSibling(i + 1u);
        }

        auto index         = m_first_free_index;
        m_first_free_index = m_tree[m_first_free_index].nextSibling();

        return index;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::insert(TreeNodeType &&node,
                                     TreeNodeIndexType parent_index,
                                     TreeNodeIndexType previous_sibling) -> TreeNodeIndexType {
        const auto index = getFreeNode();

        auto &_node = m_tree[index];
        _node       = std::forward<TreeNodeType>(node);

        _node.setParent(parent_index);

        // check if parent is real node
        if (parent_index != TreeNode::INVALID_INDEX) {
            auto &parent_node = *(std::ranges::begin(m_tree) + parent_index);

            // new node is first child
            if (parent_node.firstChild() == TreeNode::INVALID_INDEX)
                parent_node.setFirstChild(index);
            else if (previous_sibling == TreeNode::INVALID_INDEX) { // insert a beginning of childs
                _node.setNextSibling(parent_node.firstChild());
                parent_node.setFirstChild(index);
            } else { // insert at the end
                auto &prev_sibling_node = m_tree[previous_sibling];
                _node.setNextSibling(prev_sibling_node.nextSibling());
                prev_sibling_node.setNextSibling(index);
            }
        }

        return index;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::remove(TreeNodeIndexType index) -> void {
        auto &node = m_tree[index];

        if (node.parent() != TreeNode::INVALID_INDEX) {
            auto &parent = m_tree[node.parent()];

            // Remove sibling
            auto current_index = parent.firstChild();
            while (current_index != TreeNode::INVALID_INDEX) {
                auto &current_node = m_tree[current_index];

                if (current_node.nextSibling() == index) {
                    current_node.setNextSibling(node.nextSibling());
                    break;
                }
                current_index = current_node.nextSibling();
            }

            // remove parent
            if (parent.firstChild() == index) parent.setFirstChild(node.nextSibling());

            node.setParent(TreeNode::INVALID_INDEX);
        }

        auto last_index = TreeNode::INVALID_INDEX;
        auto queue      = std::deque<TreeNodeIndexType> {};
        queue.emplace_back(index);
        while (!queue.empty()) {
            auto current_index = queue.front();
            auto &current_node = m_tree[current_index];
            queue.pop_front();

            auto child_index = current_node.firstChild();
            while (child_index != TreeNode::INVALID_INDEX) {
                queue.emplace_back(child_index);
                child_index = m_tree[child_index].nextSibling();
            }

            node.invalidate();

            if (last_index != TreeNode::INVALID_INDEX)
                m_tree[last_index].setNextSibling(current_index);

            last_index = current_index;
        }

        m_tree[last_index].setNextSibling(m_first_free_index);
        m_first_free_index = index;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::markDirty(TreeNodeIndexType index, TreeNodeDirtyBitType bits)
        -> void {
        auto &node = m_tree[index];
        if (!node.dirtyBits()) {
            m_dirties.emplace_back(index);
            node.setDirtyBits(bits);
            return;
        }

        node.setDirtyBits(bits);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::operator[](TreeNodeIndexType index) noexcept -> TreeNodeType & {
        STORMKIT_EXPECTS(index < std::size(m_tree));

        return m_tree[index];
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::operator[](TreeNodeIndexType index) const noexcept
        -> const TreeNodeType & {
        STORMKIT_EXPECTS(index < std::size(m_tree));

        return m_tree[index];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::size() const noexcept -> USize {
        return std::size(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::begin() noexcept {
        return std::ranges::begin(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::begin() const noexcept {
        return std::cbegin(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::cbegin() const noexcept {
        return std::cbegin(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::end() noexcept {
        return std::ranges::end(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::end() const noexcept {
        return std::cend(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::cend() const noexcept {
        return std::cend(m_tree);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    void Tree<TreeNodeClass>::clearDirties() noexcept {
        if (std::empty(m_dirties)) return;

        for (auto i : m_dirties) { m_tree[i].setDirtyBits(0); }

        m_dirties.clear();
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::dirties() const noexcept -> std::span<const TreeNodeIndexType> {
        return m_dirties;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::genDotFile(
        std::filesystem::path filepath,
        std::function<std::string_view(std::string_view)> colorize_node) const -> void {
        auto stream = std::fstream(filepath, std::ios::out);

        stream << "digraph G { \n"
               << "    rankdir = LR\n"
               << "    bgcolor = black\n\n"
               << "    node [shape=box, fontname=\"helvetica\", fontsize=12];\n\n";

        for (auto i = 0u; i < m_first_free_index; ++i) {
            const auto name  = operator[](i).name();
            const auto dirty = bool(operator[](i).dirtyBits());

            stream << "    \"node" << i << "\" [label=\"id: " << i << " type: " << name
                   << " dirty: " << std::boolalpha << dirty << "\", style=filled,color=\""
                   << colorize_node(name) << "\"];\n";
        }

        for (auto i = 0u; i < m_first_free_index; ++i) {
            if (operator[](i).firstChild() == TreeNodeClass::INVALID_INDEX) continue;

            for (auto current = operator[](i).firstChild(); current != TreeNodeClass::INVALID_INDEX;
                 current      = operator[](current).nextSibling()) {
                stream << "    \"node" << i << "\" -> \"node" << current
                       << "\" [color=seagreen] ;\n";
            }
        }

        stream << "}" << std::flush;

        stream.close();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename TreeNodeClass>
    auto Tree<TreeNodeClass>::genDotFile(
        std::filesystem::path filepath,
        core::UInt32 highlight,
        std::function<std::string_view(std::string_view)> colorize_node) const -> void {
        std::fstream stream(filepath.string(), std::ios::out);

        stream << "digraph G { \n"
               << "    rankdir = LR\n"
               << "    bgcolor = black\n\n"
               << "    node [shape=box, fontname=\"helvetica\", fontsize=12];\n\n";

        for (auto i = 0u; i < m_first_free_index; ++i) {
            const auto name  = operator[](i).name();
            const auto dirty = bool(operator[](i).dirtyBits());
            if (i != highlight)
                stream << "    \"node" << i << "\" [label=\"id: " << i << " type: " << name
                       << " dirty: " << std::boolalpha << dirty << "\", style=filled,color=\""
                       << colorize_node(name) << "\"];\n";
            else
                stream << "    \"node" << i
                       << "\" [shape=polygon,sides=5,peripheries=3, label=\"id: " << i
                       << " type: " << name << " dirty: " << std::boolalpha << dirty
                       << "\", style=filled,color=\"" << colorize_node(name) << "\"];\n";
        }

        for (auto i = 0u; i < m_first_free_index; ++i) {
            if (operator[](i).firstChild() == TreeNodeClass::INVALID_INDEX) continue;

            for (auto current = operator[](i).firstChild(); current != TreeNodeClass::INVALID_INDEX;
                 current      = operator[](current).nextSibling()) {
                stream << "    \"node" << i << "\" -> \"nodeNode" << current
                       << "\" [color=seagreen] ;\n";
            }
        }

        stream << "}" << std::flush;

        stream.close();
    }
} // namespace stormkit::core
