// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.Core;
import stormkit.Main;
import stormkit.Log;
import stormkit.Wsi;

import App;

import <stormkit/Main/MainMacro.hpp>;

////////////////////////////////////////
////////////////////////////////////////
auto main(std::span<const std::string_view> args) -> int {
    using namespace stormkit;

    wsi::parseArgs(args);
    // core::setupSignalHandler();

    auto logger = log::Logger::createLoggerInstance<log::ConsoleLogger>();

    auto app = App {};
    return app.run(args);
}
