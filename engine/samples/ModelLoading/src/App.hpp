#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/App.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

class MainState;
class App final: public storm::core::App {
  public:
    App() noexcept;
    ~App() override;

    void run(const int argc, const char **argv) override;

  private:
    void doInitWindow();

    bool m_fullscreen     = false;
    bool m_camera_enabled = true;

    storm::window::WindowOwnedPtr m_window;
    storm::window::EventHandlerOwnedPtr m_event_handler;
    storm::engine::EngineOwnedPtr m_engine;

    std::unique_ptr<MainState> m_scene;
};
