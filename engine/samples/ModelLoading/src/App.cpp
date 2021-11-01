#include "App.hpp"
#include "Constants.hpp"
#include "Log.hpp"
#include "MainState.hpp"

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/EventHandler.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

using namespace storm;

App::App() noexcept {
    ilog("Using StormKit {}.{}.{} {} {}",
         STORMKIT_MAJOR_VERSION,
         STORMKIT_MINOR_VERSION,
         STORMKIT_PATCH_VERSION,
         STORMKIT_GIT_BRANCH,
         STORMKIT_GIT_COMMIT_HASH);
}

App::~App() = default;

auto App::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> void {
    auto model_filepath = std::filesystem::path { EXAMPLES_DATA_DIR "models/Sword.glb" };

    for (auto i = 1; i < argc; ++i) {
        if (std::filesystem::exists(argv[i])) model_filepath = std::filesystem::path { argv[1] };
        else if (std::string { argv[i] } == std::string { "--fullscreen" })
            m_fullscreen = true;
    }

    doInitWindow();

    m_engine = std::make_unique<engine::Engine>(*m_window, "ModelLoading");
    ilog("Image count: {}", m_engine->surface().bufferingCount());

    m_engine->pushState<MainState>(*m_engine, *m_window);

    while (m_window->isOpen()) {
        m_event_handler->update();

        m_engine->update();
    }

    m_engine->device().waitIdle();
}

auto App::doInitWindow() -> void {
    auto video_settings =
        window::VideoSettings { .size = core::Extentu { WINDOW_WIDTH<core::UInt32>,
                                                        WINDOW_HEIGHT<core::UInt32> } };
    auto window_style = window::WindowStyle::Close | window::WindowStyle::Resizable;

    m_window = std::make_unique<window::Window>(WINDOW_TITLE, video_settings, window_style);
    if (m_fullscreen) m_window->setFullscreenEnabled(true);

    m_event_handler = std::make_unique<window::EventHandler>(*m_window);

    m_event_handler->addCallback(window::EventType::Closed,
                                 [this]([[maybe_unused]] const auto &event) { m_window->close(); });
    m_event_handler->addCallback(window::EventType::Resized,
                                 [this]([[maybe_unused]] const auto &event) {
                                     //m_engine->recreateSwapchain();
                                 });
    m_event_handler->addCallback(window::EventType::KeyPressed, [this](const auto &event) {
        if (event.key_event.key == window::Key::Escape) m_window->close();
        else if (event.key_event.key == window::Key::F11)
            m_window->setFullscreenEnabled(!m_window->isInFullscreen());
        else if (event.key_event.key == window::Key::Space) {
            if (m_camera_enabled) m_engine->currentState<MainState>().disableCamera();
            else
                m_engine->currentState<MainState>().enableCamera();

            m_camera_enabled = !m_camera_enabled;
        }
        /*else if (event.key_event.key == window::Key::Numpad5)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::D);
        else if (event.key_event.key == window::Key::Numpad4)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::G);
        else if (event.key_event.key == window::Key::Numpad3)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::F);
        else if (event.key_event.key == window::Key::Numpad2)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Specular_Contribution);
        else if (event.key_event.key == window::Key::Numpad1)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Diffuse_Contribution);
        else if (event.key_event.key == window::Key::F10)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Emissive_Map);
        else if (event.key_event.key == window::Key::F9)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Ambiant_Occlusion_Map);
        else if (event.key_event.key == window::Key::F8)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Roughness_Map);
        else if (event.key_event.key == window::Key::F7)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Metallic_Map);
        else if (event.key_event.key == window::Key::F6)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Normal_Map);
        else if (event.key_event.key == window::Key::F5)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Albedo_Map);
        else if (event.key_event.key == window::Key::F4)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::None);
        else if (event.key_event.key == window::Key::F3)
            m_scene->toggleMSAA();
        else if (event.key_event.key == window::Key::F2)
            m_scene->toggleFreezeCamera();
        else if (event.key_event.key == window::Key::F1)
            m_scene->toggleDebugGUI();
        else if (event.key_event.key == window::Key::Space)
            m_scene->toggleMeshRotation();
        else if (event.key_event.key == window::Key::W)
            m_scene->toggleWireframe();*/
    });
}
