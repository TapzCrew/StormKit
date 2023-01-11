// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::Window - ///////////
#import "StormWindow.h"
#import "StormApplication.h"

@implementation StormWindow

/////////////////////////////////////
/////////////////////////////////////
- (BOOL)acceptsFirstResponder {
    return YES;
}

/////////////////////////////////////
/////////////////////////////////////
- (BOOL)canBecomeKeyWindow {
    return YES;
}

/////////////////////////////////////
/////////////////////////////////////
- (void)performClose:(id)sender {
    [self close];
}

@end
