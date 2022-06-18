#include <stormkit/core/Memory.mpp>

#include <stormkit/log/LogHandler.mpp>

#include <stormkit/main/MainMacro.hpp>

#include "App.mpp"

////////////////////////////////////////
////////////////////////////////////////
auto main([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> int {
    using namespace stormkit;

    core::setupSignalHandler();

    log::LogHandler::setupDefaultLogger();

    auto app = App {};
    return app.run(argc, argv);
}
