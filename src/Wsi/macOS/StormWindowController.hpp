// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



/////////// - AppKit - ///////////
#import <AppKit/NSWindow.h>

/////////// - STL - ///////////
#include <string>

/////////// - StormKit::window - ///////////
#include <storm/window/VideoSettings.hpp>
#include <storm/window/WindowStyle.hpp>

namespace storm::window::details {
    class macOSWindow;
}

@class StormView;
@interface StormWindowController: NSResponder<NSWindowDelegate>
- (id)initWithSettings:(storm::window::VideoSettings)settings
             withStyle:(storm::window::WindowStyle)style
             withTitle:(std::string)title
         withRequester:(storm::window::details::macOSWindow *)_requester;
- (void)dealloc;

- (void)setRequester:(storm::window::details::macOSWindow *)requester;

- (void)close;
- (BOOL)isOpen;
- (BOOL)isVisible;

- (void)showWindow;
- (void)hideWindow;

- (void)processEvent;
- (StormView *)nativeHandle;

- (NSSize)size;

- (BOOL)windowShouldClose:(id)sender;

- (void)windowWillClose:(id)sender;
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize;

- (void)windowDidMiniaturize:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;

- (void)setMousePosition:(NSPoint)point;
- (void)setWindowTitle:(std::string)title;

- (NSPoint)convertPoint:(NSPoint)point;
@end
