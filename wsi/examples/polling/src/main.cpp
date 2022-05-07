#ifdef STORMKIT_CXX20_MODULES
import stormkit.core.ascast;
import stormkit.core.types;
import stormkit.core.math;

import stormkit.log.loghandler;

import stormkit.wsi.window;
import stormkit.wsi.event;
import stormkit.wsi.windowstyle;
import stormkit.wsi.videosettings;
import stormkit.wsi.key;
#else
    #include <stormkit/core/AsCast.mpp>
    #include <stormkit/core/Math.mpp>
    #include <stormkit/core/Memory.mpp>
    #include <stormkit/core/Types.mpp>

    #include <stormkit/log/LogHandler.mpp>

    #include <stormkit/wsi/Event.mpp>
    #include <stormkit/wsi/Key.mpp>
    #include <stormkit/wsi/Monitor.mpp>
    #include <stormkit/wsi/Window.mpp>
    #include <stormkit/wsi/WindowStyle.mpp>
#endif

#include <stormkit/main/MainMacro.hpp>

#include <stormkit/log/LogMacro.hpp>

#include <cstdlib>

LOGGER("StormKit.Examples.Log.Polling");

auto main([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> int {
    using namespace stormkit;

    core::setupSignalHandler();

    log::LogHandler::setupDefaultLogger();

    ilog("--- Monitors ---");
    ilog("{}", wsi::Window::getMonitorSettings());

    auto window = wsi::Window { "Hello world", { 800u, 600u }, wsi::WindowStyle::All };

    auto fullscreen        = false;
    auto toggle_fullscreen = false;
    while (window.isOpen()) {
        auto event = wsi::Event {};
        while (window.pollEvent(event)) {
            switch (event.type) {
                case wsi::EventType::Closed: window.close(); break;
                case wsi::EventType::Resized: {
                    const auto &event_data = core::as<wsi::ResizedEventData>(event.data);
                    ilog("Resize event: {}", event_data.extent);
                    break;
                }
                case wsi::EventType::MouseMoved: {
                    const auto &event_data = core::as<wsi::MouseMovedEventData>(event.data);
                    ilog("Mouse move event: {}", event_data.position);
                    break;
                }
                case wsi::EventType::MouseButtonPushed: {
                    const auto &event_data = core::as<wsi::MouseButtonPushedEventData>(event.data);
                    ilog("Mouse button push event: {} {}", event_data.button, event_data.position);
                    break;
                }
                case wsi::EventType::MouseButtonReleased: {
                    const auto &event_data =
                        core::as<wsi::MouseButtonReleasedEventData>(event.data);
                    ilog("Mouse button release event: {} {}",
                         event_data.button,
                         event_data.position);
                    break;
                }
                case wsi::EventType::MouseEntered: {
                    ilog("Mouse Entered event");
                    break;
                }
                case wsi::EventType::MouseExited: {
                    ilog("Mouse Exited event");
                    break;
                }
                case wsi::EventType::LostFocus: {
                    ilog("Lost focus event");
                    break;
                }
                case wsi::EventType::GainedFocus: {
                    ilog("Gained focus event");
                    break;
                }
                case wsi::EventType::KeyPressed: {
                    const auto &event_data = core::as<wsi::KeyPressedEventData>(event.data);

                    if (event_data.key == wsi::Key::Escape) {
                        window.close();
                        ilog("Closing window");
                    } else if (event_data.key == wsi::Key::F11)
                        toggle_fullscreen = true;

                    ilog("Key pressed: {}", event_data.key);
                    break;
                }
                case wsi::EventType::KeyReleased: {
                    const auto &event_data = core::as<wsi::KeyReleasedEventData>(event.data);

                    ilog("Key release: {}", event_data.key);
                    break;
                }
                default: break;
            }
        }

        if (toggle_fullscreen) {
            fullscreen = !fullscreen;
            window.setFullscreenEnabled(fullscreen);

            toggle_fullscreen = false;
            ilog("Toggle fullscreen to: {}", fullscreen);
        }
    }

    return EXIT_SUCCESS;
}
