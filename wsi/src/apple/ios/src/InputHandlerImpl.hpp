// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef INPUT_HANDLER_IMPL_HPP
#define INPUT_HANDLER_IMPL_HPP

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/window/Fwd.hpp>
#include <storm/window/Key.hpp>

namespace storm::window {
    class STORMKIT_PRIVATE InputHandlerImpl: public core::NonCopyable {
      public:
        InputHandlerImpl();
        ~InputHandlerImpl();

        InputHandlerImpl(InputHandlerImpl &&);
        InputHandlerImpl &operator=(InputHandlerImpl &&);

        static bool isKeyPressed(Key key);
        static bool isMouseButtonPressed(MouseButton button);
        static void setMousePosition(core::Position2u position);
        static void setMousePosition(core::Position2u position, const Window &relative_to);
        static core::Position2u getMousePosition();
        static core::Position2u getMousePosition(const Window &relative_to);

        static void setVirtualKeyboardVisible(bool visible);

      private:
    };
} // namespace storm::window

#endif
