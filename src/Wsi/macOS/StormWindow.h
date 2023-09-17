// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



/////////// - AppKit - ///////////
#import <AppKit/NSWindow.h>

@interface StormWindow : NSWindow
	- (BOOL) acceptsFirstResponder;
	- (BOOL) canBecomeKeyWindow;
	- (void) performClose: (id)sender;
@end
