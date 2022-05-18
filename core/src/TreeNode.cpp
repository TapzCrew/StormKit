
#include <stormkit/core/TreeNode.mpp>

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    auto TreeNode::invalidate() noexcept -> void {
        m_parent       = INVALID_INDEX;
        m_next_sibling = INVALID_INDEX;
        m_first_child  = INVALID_INDEX;
        m_dirty_bits   = 0;
        m_name         = "";
    }
} // namespace stormkit::core
