// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include "../../AbstractWindow.hpp"

#ifdef __OBJC__
@class StormWindowController;
using StormWindowControllerOwnedPtr = StormWindowController *;
@class StormView;
using View = StormView *;
#else
using StormWindowControllerOwnedPtr = void *;
using View                          = void *;
#endif

namespace storm::window::details {
    class STORMKIT_PRIVATE macOSWindow final: public AbstractWindow {
      public:
        struct Handles {
            View view;
        };

        macOSWindow() noexcept;
        macOSWindow(std::string title, const VideoSettings &settings, WindowStyle style) noexcept;
        ~macOSWindow() override;

        macOSWindow(macOSWindow &&) noexcept;
        macOSWindow &operator=(macOSWindow &&) noexcept;

        void create(std::string title,
                    const VideoSettings &settings,
                    WindowStyle style) noexcept override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(Event &event) noexcept override;
        [[nodiscard]] bool waitEvent(Event &event) noexcept override;

        void setTitle(std::string title) noexcept override;
        void setFullscreenEnabled(bool enabled) noexcept override;

        void lockMouse() noexcept override;
        void unlockMouse() noexcept override;

        void hideMouse() noexcept override;
        void unhideMouse() noexcept override;

        [[nodiscard]] const core::Extentu &size() const noexcept override;

        [[nodiscard]] bool isOpen() const noexcept override;
        [[nodiscard]] bool isVisible() const noexcept override;

        [[nodiscard]] NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] const Handles &macOSHandles() const noexcept;

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(macOSWindow)
      private:
        void initCocoaProcess();

        StormWindowControllerOwnedPtr m_controller;

        Handles m_handles;
    };
} // namespace storm::window::details
