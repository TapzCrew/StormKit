// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#import "macOSWindow.hpp"
#import "AutoReleasePoolWrapper.hpp"
#import "StormApplication.h"
#import "StormApplicationDelegate.h"
#import "StormWindowController.hpp"
#import "Utils.hpp"

#include <storm/window/VideoSettings.hpp>

/////////// - STL - ///////////
#include <string>

/////////// - AppKit - ///////////
#import <AppKit/NSScreen.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

static auto is_process_set_as_application = false;

/////////////////////////////////////
/////////////////////////////////////
auto modeBitsPerPixel(CGDisplayModeRef mode) noexcept -> core::UInt8 {
    auto bpp = storm::core::UInt8 { 0 };

    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
    if (CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) ==
        kCFCompareEqualTo)
        bpp = 32;
    else if (CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) ==
             kCFCompareEqualTo)
        bpp = 16;
    else if (CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) ==
             kCFCompareEqualTo)
        bpp = 8;

    CFRelease(pixEnc);

    return bpp;
}

/////////////////////////////////////
/////////////////////////////////////
macOSWindow::macOSWindow() noexcept : AbstractWindow {}, m_controller { nil } {
    ensureThreadHasPool();

    initCocoaProcess();
}

/////////////////////////////////////
/////////////////////////////////////
macOSWindow::macOSWindow(std::string title,
                         const VideoSettings &settings,
                         WindowStyle style) noexcept
    : AbstractWindow {} {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
macOSWindow::~macOSWindow() {
    [m_controller close];
    [m_controller release];

    drainThreadPool();
}

/////////////////////////////////////
/////////////////////////////////////
macOSWindow::macOSWindow(macOSWindow &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::operator=(macOSWindow &&) noexcept -> macOSWindow & = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::create(std::string title,
                         const VideoSettings &settings,
                         WindowStyle style) noexcept -> void {
    initCocoaProcess();

    m_title          = std::move(title);
    m_video_settings = settings;
    m_style          = style;

    m_controller = [[StormWindowController alloc] initWithSettings:settings
                                                         withStyle:style
                                                         withTitle:title
                                                     withRequester:this];

    m_handles.view = [m_controller nativeHandle];
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::close() noexcept -> void {
    [m_controller close];

    drainThreadPool();
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::pollEvent(Event &event) noexcept -> bool {
    [m_controller processEvent];

    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::waitEvent(Event &event) noexcept -> bool {
    while (!AbstractWindow::waitEvent(event)) [m_controller processEvent];

    return true;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::setTitle(std::string title) noexcept -> void {
    m_title = std::move(title);

    [m_controller setWindowTitle:m_title];
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
auto macOSWindow::size() const noexcept -> const core::Extentu & {
    auto size = [m_controller size];

    return toStormVec(size);
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::isOpen() const noexcept -> bool {
    return [m_controller isOpen] == YES;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::isVisible() const noexcept -> bool {
    return [m_controller isVisible] == YES;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::nativeHandle() const noexcept -> NativeHandle {
    return [m_controller nativeHandle];
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::macOSHandles() const noexcept -> const Handles & {
    return m_handles;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::getDesktopModes() -> std::vector<VideoSettings> {
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) {
        NSArray *modes =
            (__bridge NSArray *)CGDisplayCopyAllDisplayModes(CGMainDisplayID(), nullptr);

        const auto mode_count = [modes count];

        video_settings.reserve(mode_count);

        for (auto i : core::range<CFIndex>(mode_count)) {
            auto mode = (CGDisplayModeRef)[modes objectAtIndex:i];

            const auto video_setting = storm::window::VideoSettings {
                .size = { static_cast<storm::core::UInt16>(CGDisplayModeGetWidth(mode)),
                          static_cast<storm::core::UInt16>(CGDisplayModeGetHeight(mode)) },
                .bpp  = modeBitsPerPixel(mode)
            };

            if (core::ranges::find(video_settings, video_setting) ==
                core::ranges::cend(video_settings))
                // if (std::find(std::cbegin(video_settings), std::cend(video_settings),
                // video_setting) ==
                //   std::cend(video_settings))
                video_settings.emplace_back(std::move(video_setting));
        }

        core::ranges::sort(video_settings, std::greater<VideoSettings> {});
        /*std::sort(std::begin(video_settings),
                  std::end(video_settings),
                  std::greater<VideoSettings> {});*/
        init = true;
    }

    return video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::getDesktopFullscreenSize() -> VideoSettings {
    static auto video_setting = storm::window::VideoSettings {};
    static auto init          = false;

    if (!init) {
        const auto modes = getDesktopModes();

        for (const auto &mode : modes) {
            video_setting.size.width  = std::max(video_setting.size.width, mode.size.width);
            video_setting.size.height = std::max(video_setting.size.height, mode.size.height);
            video_setting.size.depth  = std::max(video_setting.size.depth, mode.size.depth);
        }

        init = true;
    }

    return video_setting;
}
/////////////////////////////////////
/////////////////////////////////////
auto macOSWindow::initCocoaProcess() -> void {
    if (!is_process_set_as_application) {
        [StormApplication sharedApplication];

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        if (![[StormApplication sharedApplication] delegate])
            [[StormApplication sharedApplication]
                setDelegate:[[StormApplicationDelegate alloc] init]];

        [[StormApplication sharedApplication] finishLaunching];

        is_process_set_as_application = true;
    }
}
