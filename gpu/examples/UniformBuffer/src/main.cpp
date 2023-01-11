// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
import std;

import stormkit.Core;
import stormkit.Main;
import stormkit.Log;

import App;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Log.hpp>
    #include <stormkit/Main.hpp>

    #include "App.mpp"
#endif

////////////////////////////////////////
////////////////////////////////////////
auto main(const int argc, const char **argv) -> int {
    using namespace stormkit;

    core::setupSignalHandler();

    auto logger = log::Logger::createLoggerInstance<log::ConsoleLogger>();

    auto app = App {};
    return app.run(argc, argv);
}
