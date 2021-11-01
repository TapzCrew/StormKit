#include <storm/core/TreeNode.hpp>

using namespace storm::core;

/////////////////////////////////////
/////////////////////////////////////
void TreeNode::invalidate() noexcept {
    m_parent       = INVALID_INDEX;
    m_next_sibling = INVALID_INDEX;
    m_first_child  = INVALID_INDEX;
    m_dirty_bits   = 0;
    m_name         = "";
}
