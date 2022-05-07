#include <storm/engine/render/framegraph/FrameGraphNode.hpp>

#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

using namespace storm;
using namespace storm::engine;

namespace storm::engine::details {
    ////////////////////////////////////////
    ////////////////////////////////////////
    FrameGraphNode::FrameGraphNode(std::string name, FrameGraphNodeID id, QueueFlag flags) noexcept
        : m_name { std::move(name) }, m_id { id }, m_queue_flags { flags } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    FrameGraphNode::~FrameGraphNode() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    FrameGraphNode::FrameGraphNode(FrameGraphNode &&) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto FrameGraphNode::operator=(FrameGraphNode &&) noexcept -> FrameGraphNode & = default;
} // namespace storm::engine::details
