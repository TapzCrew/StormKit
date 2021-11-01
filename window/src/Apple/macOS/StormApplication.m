// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::Window - ///////////
#import "StormApplication.h"

@implementation StormApplication

/////////////////////////////////////
/////////////////////////////////////
+ (void)processEvent {
    [StormApplication sharedApplication];

    NSEvent *event = nil;

    do {
        event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:[NSDate distantPast]
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES];

        [NSApp sendEvent:event];
    } while (event != nil);
}

@end
