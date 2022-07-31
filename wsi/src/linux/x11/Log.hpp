// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#pragma once

#include <stormkit/log/LogMacro.hpp>

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.details.x11.log;

/////////// - StormKit::log - ///////////
import stormkit.log.loghandler;

export {
#else
    /////////// - StormKit::log - ///////////
    #include <stormkit/log/LogHandler.mpp>
#endif

    namespace stormkit::wsi::details::x11 {
        LOGGER("StormKit.Wsi.Details.X11");
    } // namespace stormkit::wsi::details::x11

#if defined(STORMKIT_CXX20_MODULES)
}
#endif