// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - AppKit - ///////////
#import <AppKit/NSView.h>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

namespace storm::window::details {
    class macOSWindow;
}

@interface StormView: NSView
- (BOOL)acceptsFirstResponder;
- (id)initWithFrame:(NSRect)frame
      withRequester:(storm::window::details::macOSWindow *)_requester
         withWindow:(NSWindow *)window;

- (NSWindow *)myWindow;
- (void)mouseDown:(NSEvent *)event;
- (void)rightMouseDown:(NSEvent *)event;
- (void)otherMouseDown:(NSEvent *)event;
- (void)handleMouseDown:(NSEvent *)event;

- (void)mouseUp:(NSEvent *)event;
- (void)rightMouseUp:(NSEvent *)event;
- (void)otherMouseUp:(NSEvent *)event;
- (void)handleMouseUp:(NSEvent *)event;

- (void)mouseMoved:(NSEvent *)event;
- (void)mouseDragged:(NSEvent *)event;
- (void)rightMouseDragged:(NSEvent *)event;
- (void)handleMouseMove:(NSEvent *)event;

- (void)mouseEntered:(NSEvent *)event;
- (void)mouseExited:(NSEvent *)event;

- (void)keyDown:(NSEvent *)event;
- (void)keyUp:(NSEvent *)event;

- (void)updateTrackingAreas;

- (void)setRequester:(storm::window::details::macOSWindow *)_requester;

- (NSPoint)relativeToGlobal:(NSPoint)point;
- (void)setNativeEventRetriever:(void *)native_event;
@end
