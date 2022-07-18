#pragma once

#include <stormkit/core/App.mpp>
#include <stormkit/core/Memory.mpp>

#include <stormkit/wsi/Window.mpp>

#include <stormkit/image/Image.mpp>

#include <stormkit/entities/EntityManager.mpp>

#include "Renderer.mpp"

class UpdateBoardSystem;
class App: public stormkit::core::App {
  public:
    App();
    ~App() override;

    auto run(const int argc, const char **argv) -> stormkit::core::Int32 override;

  private:
    auto doInitWindow() -> void;

    auto handleKeyboard(const stormkit::wsi::KeyReleasedEventData &event) -> void;
    auto handleMouse(const stormkit::wsi::MouseButtonPushedEventData &event) -> void;
    auto createCell(stormkit::core::UInt32 x, stormkit::core::UInt32 y) -> void;

    stormkit::wsi::WindowOwnedPtr m_window;

    bool m_fullscreen_enabled = false;
    bool m_is_on_edit_mode    = true;

    std::unique_ptr<Renderer> m_renderer;

    stormkit::entities::EntityManager m_entities;

    stormkit::image::Image m_board;

    UpdateBoardSystem *m_update_system = nullptr;
};
