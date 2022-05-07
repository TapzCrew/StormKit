#include <storm/engine/render/framegraph/FrameGraph.hpp>
#include <storm/engine/render/framegraph/FrameGraphBuilder.hpp>
#include <storm/engine/render/framegraph/FrameGraphNode.hpp>

using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::FrameGraphBuilder(FrameGraph &frame_graph,
                                     details::FrameGraphNode &frame_pass) noexcept
    : m_frame_graph { frame_graph }, m_frame_pass { frame_pass } {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::~FrameGraphBuilder() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::FrameGraphBuilder(FrameGraphBuilder &&drawable) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraphBuilder::operator=(FrameGraphBuilder &&drawable) noexcept
    -> FrameGraphBuilder &      = default;

/////////////////////////////////////
/////////////////////////////////////
auto FrameGraphBuilder::read(FrameGraphResourceID input) -> FrameGraphResourceID {
    auto &frame_graph = m_frame_graph.get();
    auto &frame_pass  = m_frame_pass.get();

    auto &resource = frame_graph.getResource(input);
    resource.m_read_in.emplace_back(frame_pass.id());

    frame_pass.m_reads.emplace_back(input);

    return input;
}

/////////////////////////////////////
/////////////////////////////////////
auto FrameGraphBuilder::write(FrameGraphResourceID output) -> FrameGraphResourceID {
    auto &frame_graph = m_frame_graph.get();
    auto &frame_pass  = m_frame_pass.get();

    auto &resource = frame_graph.getResource(output);
    resource.m_writed_in.emplace_back(frame_pass.id());

    frame_pass.m_writes.emplace_back(output);

    return output;
}
