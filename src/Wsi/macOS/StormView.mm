// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::Window - ///////////
#import "StormView.hpp"
#import "Utils.hpp"
#import "macOSWindow.hpp"

/////////// - AppKit - ///////////
#import <AppKit/NSScreen.h>
#import <AppKit/NSTrackingArea.h>
#import <AppKit/NSWindow.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

@implementation StormView {
    NSTrackingArea *trackingArea;
    macOSWindow *requester;
    NSWindow *windowA;
    BOOL isMouseInside;
    void *nativeEvent;
}

/////////////////////////////////////
/////////////////////////////////////
- (NSWindow *)myWindow {
    return windowA;
}
/////////////////////////////////////
/////////////////////////////////////
- (BOOL)acceptsFirstResponder {
    return YES;
}

/////////////////////////////////////
/////////////////////////////////////
- (id)initWithFrame:(NSRect)frame
      withRequester:(macOSWindow *)_requester
         withWindow:(NSWindow *)window {
    self = [super initWithFrame:frame];

    windowA       = window;
    isMouseInside = NO;
    nativeEvent   = nullptr;
    requester     = _requester;

    return self;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)setRequester:(macOSWindow *)_requester {
    requester = _requester;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseDown:(NSEvent *)event {
    /*if(nativeEvent)
     *reinterpret_cast<NSEvent*>(nativeEvent) = *event;*/

    [self handleMouseDown:event];

    [[self nextResponder] mouseDown:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)rightMouseDown:(NSEvent *)event {
    [self handleMouseDown:event];

    [[self nextResponder] mouseDown:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)otherMouseDown:(NSEvent *)event {
    [self handleMouseDown:event];

    [[self nextResponder] mouseDown:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)handleMouseDown:(NSEvent *)event {
    auto mouse_key = toStormMouseButton([event buttonNumber]);

    requester->mouseDownEvent(mouse_key, event.locationInWindow.x, event.locationInWindow.y);
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseUp:(NSEvent *)event {
    [self handleMouseUp:event];

    [[self nextResponder] mouseUp:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)rightMouseUp:(NSEvent *)event {
    [self handleMouseUp:event];

    [[self nextResponder] mouseUp:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)otherMouseUp:(NSEvent *)event {
    [self handleMouseUp:event];

    [[self nextResponder] mouseUp:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)handleMouseUp:(NSEvent *)event {
    auto mouse_key = toStormMouseButton([event buttonNumber]);

    requester->mouseUpEvent(mouse_key, event.locationInWindow.x, event.locationInWindow.y);
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseMoved:(NSEvent *)event {
    [self handleMouseMove:event];

    [[self nextResponder] mouseMoved:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseDragged:(NSEvent *)event {
    [self handleMouseMove:event];

    [[self nextResponder] mouseDragged:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)rightMouseDragged:(NSEvent *)event {
    [self handleMouseMove:event];

    [[self nextResponder] rightMouseDragged:event];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)handleMouseMove:(NSEvent *)event {
    if (isMouseInside) {
        auto point = toStormVec(event.locationInWindow);

        point.y = requester->videoSettings().size.height - point.y;

        requester->mouseMoveEvent(point.x, point.y);
    }
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseEntered:(NSEvent *)event {
    isMouseInside = YES;

    requester->mouseEnteredEvent();
}

/////////////////////////////////////
/////////////////////////////////////
- (void)mouseExited:(NSEvent *)event {
    isMouseInside = NO;

    requester->mouseExitedEvent();
}

/////////////////////////////////////
/////////////////////////////////////
- (void)keyDown:(NSEvent *)event {
    auto string = [event charactersIgnoringModifiers];
    auto key    = storm::window::Key::Unknow;

    if ([string length] > 0) key = localizedKeyToStormKey([string characterAtIndex:0]);

    if (key == storm::window::Key::Unknow) key = nonLocalizedKeytoStormKey(event.keyCode);

    requester->keyDownEvent(key, [string characterAtIndex:0]);
}

/////////////////////////////////////
/////////////////////////////////////
- (void)keyUp:(NSEvent *)event {
    auto string = [event charactersIgnoringModifiers];
    auto key    = storm::window::Key::Unknow;

    if ([string length] > 0) key = localizedKeyToStormKey([string characterAtIndex:0]);

    if (key == storm::window::Key::Unknow) key = nonLocalizedKeytoStormKey(event.keyCode);

    requester->keyUpEvent(key, [string characterAtIndex:0]);
}

/////////////////////////////////////
/////////////////////////////////////
- (void)updateTrackingAreas {
    [super updateTrackingAreas];
    if (trackingArea != nil) {
        [self removeTrackingArea:trackingArea];
        [trackingArea release];
    }

    NSUInteger opts = (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways |
                       NSTrackingMouseMoved | NSTrackingEnabledDuringMouseDrag);
    trackingArea    = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                options:opts
                                                  owner:self
                                               userInfo:nil];
    [self addTrackingArea:trackingArea];
}

/////////////////////////////////////
/////////////////////////////////////
- (NSPoint)convertPointToScreen:(NSPoint)point {
    NSRect rect = NSZeroRect;
    rect.origin = point;
    rect        = [[self window] convertRectToScreen:rect];
    return rect.origin;
}

/////////////////////////////////////
/////////////////////////////////////
- (NSPoint)relativeToGlobal:(NSPoint)point {
    point.y = [self frame].size.height - point.y;

    point = [self convertPoint:point toView:self];
    point = [self convertPoint:point toView:nil];

    point = [self convertPointToScreen:point];

    const float screenHeight = [[[self window] screen] frame].size.height;
    point.y                  = screenHeight - point.y;

    return point;
}

/////////////////////////////////////
/////////////////////////////////////
- (CGDirectDisplayID)displayId {
    NSScreen *screen    = [[self window] screen];
    NSNumber *displayId = [[screen deviceDescription] objectForKey:@"NSScreenNumber"];

    return [displayId intValue];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)setNativeEventRetriever:(void *)native_event {
    nativeEvent = native_event;
}

@end
