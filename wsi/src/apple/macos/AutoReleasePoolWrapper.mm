// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#import "AutoReleasePoolWrapper.hpp"

/////////// - Foundation - ///////////
#import <Foundation/NSAutoreleasePool.h>

thread_local NSAutoreleasePool *pool = nullptr;

/////////////////////////////////////
/////////////////////////////////////
auto createNewPool() noexcept -> void {
    pool = [[NSAutoreleasePool alloc] init];
}

/////////////////////////////////////
/////////////////////////////////////
auto destroyPool() noexcept -> void {
    [pool drain];
}

/////////////////////////////////////
/////////////////////////////////////
extern "C" auto ensureThreadHasPool() noexcept -> void {
    if (pool == nullptr) { createNewPool(); }
}

/////////////////////////////////////
/////////////////////////////////////
extern "C" auto drainThreadPool() noexcept -> void {
    destroyPool();
    createNewPool();
}
