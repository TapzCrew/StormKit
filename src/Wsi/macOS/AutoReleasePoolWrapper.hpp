// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



/////////// - StormKit::Core - ///////////
#include <storm/core/Platform.hpp>

#ifdef __cplusplus
extern "C" {
#endif
STORMKIT_PRIVATE void ensureThreadHasPool() noexcept;
STORMKIT_PRIVATE void drainThreadPool() noexcept;
#ifdef __cplusplus
}
#endif
