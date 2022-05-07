#include <storm/log/LogHandler.hpp>

#include <storm/window/Window.hpp>

#include <storm/main/Main.hpp>

#include <storm/core/RingBuffer.hpp>

#include <clocale>

int main([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) {
    using namespace storm;
    using namespace storm::window;
    using namespace storm::log;

    std::setlocale(LC_ALL, "en_US.utf8");

    LogHandler::setupDefaultLogger();

    auto window =
        Window { "Hello world", VideoSettings { .size = { 800u, 600u } }, WindowStyle::All };

    for (const auto &setting : Window::getDesktopModes())
        LogHandler::ilog("Screen: {}", setting.size);

    LogHandler::ilog("Fullscreen resolution: {}", Window::getDesktopFullscreenSize().size);

    auto fullscreen        = false;
    auto toggle_fullscreen = false;
    while (window.isOpen()) {
        auto event = Event {};
        while (window.pollEvent(event)) {
            switch (event.type) {
                case EventType::Closed: window.close(); break;
                case EventType::Resized: {
                    const auto size = event.resized_event;
                    LogHandler::ilog("Resize event: {}:{}", size.width, size.height);
                    break;
                }
                case EventType::MouseMoved: {
                    const auto position = event.mouse_event;
                    LogHandler::ilog("Mouse move event: {}:{}", position.x, position.y);
                    break;
                }
                case EventType::MouseButtonPushed: {
                    const auto button = event.mouse_event;
                    LogHandler::ilog("Mouse button push event: {} {}:{}",
                                     button.button,
                                     button.x,
                                     button.y);
                    break;
                }
                case EventType::MouseButtonReleased: {
                    const auto button = event.mouse_event;
                    LogHandler::ilog("Mouse button release event: {} {}:{}",
                                     button.button,
                                     button.x,
                                     button.y);
                    break;
                }
                case EventType::MouseEntered: {
                    LogHandler::ilog("Mouse Entered event");
                    break;
                }
                case EventType::MouseExited: {
                    LogHandler::ilog("Mouse Exited event");
                    break;
                }
                case EventType::LostFocus: {
                    LogHandler::ilog("Lost focus event");
                    break;
                }
                case EventType::GainedFocus: {
                    LogHandler::ilog("Gained focus event");
                    break;
                }
                case EventType::KeyPressed: {
                    const auto key = event.key_event.key;

                    if (event.key_event.key == Key::Escape) {
                        window.close();
                        LogHandler::ilog("Closing window");
                    } else if (event.key_event.key == Key::F11)
                        toggle_fullscreen = true;

                    LogHandler::ilog("Key pressed: {}", key);
                    break;
                }
                case EventType::KeyReleased: {
                    const auto key = event.key_event.key;

                    if (event.key_event.key == Key::Escape) {
                        window.close();
                        break;
                    }

                    LogHandler::ilog("Key release: {}", key);
                    break;
                }
                default: break;
            }
        }

        if (toggle_fullscreen) {
            fullscreen = !fullscreen;
            window.setFullscreenEnabled(fullscreen);

            toggle_fullscreen = false;
            LogHandler::ilog("Toggle fullscreen to: {}", fullscreen);
        }
    }

    return EXIT_SUCCESS;
}
