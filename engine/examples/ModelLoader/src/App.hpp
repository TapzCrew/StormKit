#pragma once

#include <stormkit/core/App.hpp>

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/engine/Fwd.hpp>

#include <stormkit/entities/Entity.hpp>
#include <stormkit/entities/Fwd.hpp>

class App: public stormkit::core::App {
  public:
    App();
    ~App() override;

    auto run(const int argc, const char **argv) -> stormkit::core::Int32 override;

  private:
    auto doInitEngine() -> void;
    auto doInitMesh() -> void;

    stormkit::wsi::WindowOwnedPtr m_window;
    bool m_fullscreen_enabled = false;

    stormkit::engine::EngineOwnedPtr m_engine;

    stormkit::entities::Entity m_mesh;
};
