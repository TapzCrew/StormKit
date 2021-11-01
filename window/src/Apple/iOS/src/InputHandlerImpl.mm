#import "InputHandlerImpl.hpp"

#include <storm/window/Event.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl() {
}

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::~InputHandlerImpl() = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl &InputHandlerImpl::operator=(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isKeyPressed(Key key) {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isMouseButtonPressed(MouseButton button) {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2u position) {
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2u position, const Window &relative_to) {
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePosition() {
    return core::makeNamed<core::Position2u>(core::Vector2u { 0u, 0u });
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePosition(const Window &relative_to) {
    return core::makeNamed<core::Position2u>(core::Vector2u { 0u, 0u });
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) {
    // not supported
}
