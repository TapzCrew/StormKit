// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#import "StormWindowController.hpp"
#import "AutoReleasePoolWrapper.hpp"
#import "StormApplication.h"
#import "StormView.hpp"
#import "StormWindow.h"
#include "macOSWindow.hpp"

/////////// - Metal - ///////////
#import <Metal/Metal.h>

/////////// - QuartzCore - ///////////
#import <QuartzCore/CAMetalLayer.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

STORMKIT_PRIVATE auto fromStdString(const std::string &str) noexcept -> NSString * {
    return [NSString stringWithCString:str.c_str() encoding:[NSString defaultCStringEncoding]];
}

@implementation StormWindowController {
    macOSWindow *requester;
    NSWindow *window;
    StormView *view;
    BOOL is_open;
}

/////////////////////////////////////
/////////////////////////////////////
- (id)initWithSettings:(VideoSettings)settings
             withStyle:(WindowStyle)style
             withTitle:(std::string)title
         withRequester:(macOSWindow *)_requester {
    [super init];

    if (self) {
        view   = nil;
        window = nil;

        auto window_style = NSUInteger(NSWindowStyleMaskTitled);
        if ((style & WindowStyle::Close) == WindowStyle::Close)
            window_style |= NSWindowStyleMaskClosable;

        if ((style & WindowStyle::Resizable) == WindowStyle::Resizable)
            window_style |= NSWindowStyleMaskResizable;

        if ((style & WindowStyle::Minimizable) == WindowStyle::Minimizable)
            window_style |= NSWindowStyleMaskMiniaturizable;

        const auto rect = NSMakeRect(0, 0, settings.size.width, settings.size.height);
        window          = [[StormWindow alloc] initWithContentRect:rect
                                                styleMask:window_style
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];

        auto frame = [window convertRectToBacking:[[window contentView] frame]];

        view = [[StormView alloc] initWithFrame:frame withRequester:_requester withWindow:window];
        [view setLayer:[CAMetalLayer layer]];
        [view setWantsLayer:YES];

        [window setReleasedWhenClosed:NO];
        [window makeFirstResponder:self];

        [window setContentView:view];
        [window setOpaque:YES];
        [window setTitle:fromStdString(title)];

        [window setDelegate:self];
        [window center];
        [window setAutodisplay:YES];
        [window orderFrontRegardless];

        // if (fullscreen) [window toggleFullScreen:self];

        requester = _requester;

        is_open = YES;
    }

    return self;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)dealloc {
    [self close];

    [window release];
    [view release];

    [super dealloc];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)setRequester:(macOSWindow *)_requester {
    requester = _requester;
    [view setRequester:requester];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)close {
    [window close];
    [window setDelegate:nil];

    [self setRequester:nullptr];
}

/////////////////////////////////////
/////////////////////////////////////
- (BOOL)isOpen {
    return is_open;
}

/////////////////////////////////////
/////////////////////////////////////
- (BOOL)isVisible {
    return [window isVisible];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)showWindow {
    [window orderOut:nil];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)hideWindow {
    [window makeKeyAndOrderFront:nil];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)processEvent {
    [StormApplication processEvent];
    drainThreadPool();

    // CGAssociateMouseAndMouseCursorPosition(TRUE);
}

/////////////////////////////////////
/////////////////////////////////////
- (StormView *)nativeHandle {
    return view;
}

/////////////////////////////////////
/////////////////////////////////////
- (NSSize)size {
    return [window contentRectForFrameRect:window.frame].size;
}

/////////////////////////////////////
/////////////////////////////////////
- (BOOL)windowShouldClose:(id)sender {
    is_open = NO;
    return YES;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)windowWillClose:(id)sender {
    requester->closeEvent();
}

/////////////////////////////////////
/////////////////////////////////////
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize {
    requester->resizeEvent(frameSize.width, frameSize.height);

    return frameSize;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)windowDidMiniaturize:(NSNotification *)notification {
    requester->minimizeEvent();
}

/////////////////////////////////////
/////////////////////////////////////
- (void)windowDidDeminiaturize:(NSNotification *)notification {
    requester->maximizeEvent();
}

/////////////////////////////////////
/////////////////////////////////////
- (void)setMousePosition:(NSPoint)point {
    [view setMousePosition:point];
}

/////////////////////////////////////
/////////////////////////////////////
- (void)setWindowTitle:(std::string)title {
    [window setTitle:[NSString stringWithCString:title.c_str()
                                        encoding:[NSString defaultCStringEncoding]]];
}

/////////////////////////////////////
/////////////////////////////////////
- (NSPoint)convertPoint:(NSPoint)point {
    NSPoint pointInView = [view convertPoint:point fromView:nil];

    return pointInView;
}

@end
