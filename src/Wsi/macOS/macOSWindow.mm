// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#import "macOSWindow.hpp"
#import "StormKitApplication.h"
#import "StormKitWindow.hpp"

#import <AppKit/NSScreen.h>
#import <IOKit/graphics/IOGraphicsLib.h>

#include <bit>
#include <cstdint>
#include <string>
#include <vector>

namespace stormkit::wsi::macos {
    namespace {
        static auto is_process_set_as_application = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    macOSWindow::macOSWindow(std::string title,
                             std::uint32_t width,
                             std::uint32_t height,
                             int style) noexcept
        : m_controller { nil }, m_handles { .view = nil }, m_title { std::move(title) },
          m_width { width }, m_height { height } {
        initCocoaProcess();

        m_controller = [[StormKitWindowController alloc]
            initWithWidth:width
               withHeight:height
                withStyle:static_cast<int>(style)
                withTitle:m_title
            withRequester:this];

        m_handles.view = [m_controller nativeHandle];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    macOSWindow::~macOSWindow() {
        [m_controller close];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    macOSWindow::macOSWindow(macOSWindow &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::operator=(macOSWindow &&) noexcept -> macOSWindow & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::pollEvent() noexcept -> void {
        [m_controller processEvent];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::waitEvent() noexcept -> void {
        [m_controller processEvent];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setTitle(std::string title) noexcept -> void {
        m_title = std::move(title);

        [m_controller setWindowTitle:m_title];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setExtent(std::uint32_t width, std::uint32_t height) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setFullscreenEnabled(bool fullscreen) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::lockMouse() noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::unlockMouse() noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::hideMouse() noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::unhideMouse() noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::width() const noexcept -> std::uint32_t {
        auto size = [m_controller size];

        return size.width;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::height() const noexcept -> std::uint32_t {
        auto size = [m_controller size];

        return size.height;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::macOSHandles() const noexcept -> const Handles & {
        return m_handles;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setKeyRepeatEnabled([[maybe_unused]] bool enabled) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setMousePosition([[maybe_unused]] std::int32_t x,
                                       [[maybe_unused]] std::int32_t y) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::setMousePositionOnDesktop([[maybe_unused]] std::uint32_t x,
                                                [[maybe_unused]] std::uint32_t y) noexcept -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto macOSWindow::getMonitorSettings() -> std::vector<Monitor> {
        // TODO handle multiple monitors
        auto display_id  = CGMainDisplayID();
        auto modes       = (__bridge NSArray *)CGDisplayCopyAllDisplayModes(display_id, nullptr);
        auto device_info = (__bridge NSDictionary *)
            IODisplayCreateInfoDictionary(CGDisplayIOServicePort(display_id),
                                          kIODisplayOnlyPreferredName);

        const auto mode_count = [modes count];

        auto monitors = std::vector<Monitor> {};
        auto &monitor = monitors.emplace_back();
        monitor.extents.reserve(mode_count);
        monitor.flags  = Monitor::Flags::Primary;
        monitor.handle = (void *)display_id;

        auto screen_name = @"";
        auto localized_names =
            [device_info objectForKey:[NSString stringWithUTF8String:kDisplayProductName]];
        if ([localized_names count] > 0) [[likely]]
            screen_name =
                [localized_names objectForKey:[[localized_names allKeys] objectAtIndex:0]];

        monitor.name = [screen_name UTF8String];

        for (auto i = CFIndex { 0 }; i < mode_count; ++i) {
            auto mode = (__bridge CGDisplayModeRef)([modes objectAtIndex:i]);

            monitor.extents.emplace_back(CGDisplayModeGetWidth(mode), CGDisplayModeGetHeight(mode));
        }

        return monitors;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto initCocoaProcess() -> void {
        if (!is_process_set_as_application) {
            [StormKitApplication sharedApplication];

            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
            [NSApp activateIgnoringOtherApps:YES];

            if (![[StormKitApplication sharedApplication] delegate])
                [[StormKitApplication sharedApplication]
                    setDelegate:[[StormKitApplicationDelegate alloc] init]];

            [[StormKitApplication sharedApplication] finishLaunching];

            is_process_set_as_application = true;
        }
    }
} // namespace stormkit::wsi::macos
