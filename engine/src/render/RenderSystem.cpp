/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>
#include <storm/entities/MessageBus.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

#include <storm/engine/render/DrawableComponent.hpp>
#include <storm/engine/render/RenderSystem.hpp>

#include <storm/engine/render/framegraph/FrameGraph.hpp>

using namespace storm;
using namespace storm::engine;

struct CameraData {
    core::Matrix projection;
    core::Matrix view;
};

/////////////////////////////////////
/////////////////////////////////////
RenderSystem::RenderSystem(Engine &engine, entities::EntityManager &manager, ComponentTypes types)
    : entities::System { manager, 0, std::move(types) }, m_engine { engine } {
}

/////////////////////////////////////
/////////////////////////////////////
RenderSystem::~RenderSystem() = default;

/////////////////////////////////////
/////////////////////////////////////
RenderSystem::RenderSystem(RenderSystem &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto RenderSystem::operator=(RenderSystem &&) noexcept -> RenderSystem & = default;

/////////////////////////////////////
/////////////////////////////////////
auto RenderSystem::render(render::Surface::Frame &frame) -> void {
    auto &engine      = m_engine.get();
    auto &surface     = engine.surface();
    auto &profiler    = engine.profiler();
    const auto extent = surface.extent();

    auto &frame_graph = engine.frameGraph();

    profiler.beginStage("FrameGraph setup");

    frame_graph.beginNewFrame(frame);

    const auto backbuffer_descriptor =
        TextureDescriptor { .type    = render::TextureType::T2D,
                            .format  = surface.pixelFormat(),
                            .extent  = extent,
                            .samples = render::SampleCountFlag::C1_BIT,
                            .usage   = render::TextureUsage::Color_Attachment };

    const auto backbuffer_id =
        frame_graph.setRetainedResource("backbuffer",
                                        backbuffer_descriptor,
                                        surface.textures()[frame.texture_index]);
    frame_graph.setBackbuffer(backbuffer_id);
    setupFrameGraph(frame_graph, backbuffer_id);

    profiler.endStage("FrameGraph setup");

    profiler.beginStage("FrameGraph compilation");
    frame_graph.build();
    profiler.endStage("FrameGraph compilation");

    profiler.beginStage("FrameGraph execution");
    frame_graph.execute();
    profiler.endStage("FrameGraph execution");
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderSystem::onMessageReceived(const entities::Message &message) -> void {
    if (message.id == entities::EntityManager::ADDED_ENTITY_MESSAGE_ID)
        for (auto e : message.entities) addEntity(e);
    else if (message.id == entities::EntityManager::REMOVED_ENTITY_MESSAGE_ID)
        for (auto e : message.entities) removeEntity(e);
}

/////////////////////////////////////
/////////////////////////////////////
auto RenderSystem::addEntity(entities::Entity e) -> void {
    if (!m_manager->hasComponent<DrawableComponent>(e)) return;

    if (core::ranges::any_of(m_to_be_added, [e](auto _e) { return _e == e; })) return;

    m_to_be_added.emplace_back(e);
    m_need_update = true;
}

auto RenderSystem::removeEntity(entities::Entity e) -> void {
    if (!m_manager->hasComponent<DrawableComponent>(e)) return;

    if (core::ranges::any_of(m_to_be_removed, [e](auto _e) { return _e == e; })) return;

    m_to_be_removed.emplace_back(e);
    m_need_update = true;
}
