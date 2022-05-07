/////////// - StormKit::window - ///////////
#include <storm/window/Event.hpp>
#include <storm/window/EventHandler.hpp>
#include <storm/window/Window.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(Window &window) : m_window { &window } {
}

/////////////////////////////////////
/////////////////////////////////////
EventHandler::~EventHandler() = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(const EventHandler &) = default;

/////////////////////////////////////
/////////////////////////////////////
auto EventHandler::operator=(const EventHandler &) -> EventHandler & = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(EventHandler &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto EventHandler::operator=(EventHandler &&) noexcept -> EventHandler & = default;

/////////////////////////////////////
/////////////////////////////////////
auto EventHandler::update() -> void {
    auto event = Event {};

    while (m_window->pollEvent(event)) {
        auto event_type = event.type;

        for (auto &callback : m_callback[event_type]) callback(event);
    }
}
