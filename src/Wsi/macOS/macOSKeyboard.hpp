// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Key.hpp>
#include <storm/window/Keyboard.hpp>

/////////// - Cocoa - ///////////
#include <Carbon/Carbon.h>

/////////// - IOKit - ///////////
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>

namespace storm::window::details {
    class macOSWindow;
    using macOSWindowConstPtr = const macOSWindow *;

    class STORMKIT_PRIVATE macOSKeyboard final: public Keyboard {
      public:
        explicit macOSKeyboard(const AbstractWindow &window);
        ~macOSKeyboard() override;

        macOSKeyboard(macOSKeyboard &&) noexcept;
        macOSKeyboard &operator=(macOSKeyboard &&) noexcept;

        bool isKeyPressed(Key key) const noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

        ALLOCATE_HELPERS(macOSKeyboard)
      private:
        macOSWindowConstPtr m_macos_window;

        static void initIOHID();
        static void initializeKeyboard();
        static void loadKeyboard(IOHIDDeviceRef keyboard);
        static void loadKey(IOHIDElementRef key);
        static CFDictionaryRef copyDevicesMask(core::UInt32 page, core::UInt32 usage);
        static CFSetRef copyDevices(core::UInt32 page, core::UInt32 usage);

        using IOHIDElements = std::vector<IOHIDElementRef>;

        static inline CFDataRef m_layout_data    = nullptr;
        static inline UCKeyboardLayout *m_layout = nullptr;
        static inline IOHIDManagerRef m_manager  = nullptr;

        static inline IOHIDElements m_keys[static_cast<core::ArraySize>(Key::Keycount)] = {};
    };
} // namespace storm::window::details
