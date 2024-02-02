// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.Core;
import stormkit.Main;
import stormkit.Log;
import stormkit.Wsi;

import App;

#include <stormkit/Main/MainMacro.hpp>
#include <stormkit/Core/PlatformMacro.hpp>

////////////////////////////////////////
////////////////////////////////////////
auto main(std::span<const std::string_view> args) -> int {
    using namespace stormkit;

    wsi::parseArgs(args);
    // core::setupSignalHandler();

    auto logger = log::Logger::createLoggerInstance<log::ConsoleLogger>();
    logger.ilog("Using StormKit {}.{}.{} (branch: {}, commit_hash: {}), built with {}",
         core::STORMKIT_MAJOR_VERSION,
         core::STORMKIT_MINOR_VERSION,
         core::STORMKIT_PATCH_VERSION,
         core::STORMKIT_GIT_BRANCH,
         core::STORMKIT_GIT_COMMIT_HASH,
         STORMKIT_COMPILER);

    try {
        auto app = App {};
        return app.run(args);
    } catch (const std::exception& e) {
        logger.flog("{}", e.what());
        return -1;
    } catch (...) {
        logger.flog("Uncaught exception occured !");
        return -1;
    }
}
