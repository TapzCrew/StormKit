#include "App.mpp"
#include "Constants.mpp"

#include <stormkit/core/Configure.mpp>

#include <stormkit/log/LogHandler.mpp>

#include <stormkit/wsi/Window.mpp>

#include <stormkit/engine/Engine.mpp>
#include <stormkit/engine/render/MeshComponent.mpp>

using namespace stormkit;

App::App() {
    doInitEngine();
    doInitMesh();
}

App::~App() = default;

auto App::run(const int argc, const char **argv) -> core::Int32 {
    return m_engine->run(argc, argv);
}

auto App::doInitEngine() -> void {
    const auto window_style = wsi::WindowStyle::All;

    m_window =
        std::make_unique<wsi::Window>(WINDOW_TITLE, core::ExtentU { 800u, 600u }, window_style);

    m_engine = std::make_unique<engine::Engine>(*m_window);
}

auto App::doInitMesh() -> void {
    auto &world = m_engine->world();

    m_mesh = world.makeEntity();

    auto &mesh_component     = world.addComponent<engine::MeshComponent>(m_mesh);
    mesh_component.submeshes = { engine::SubMesh { .vertices = { VERTICES } } };
}
