#include <stormkit/core/Memory.hpp>

#include <stormkit/log/ConsoleLogger.hpp>
#include <stormkit/log/Logger.hpp>

#include <stormkit/main/MainMacro.hpp>

#include "App.hpp"

////////////////////////////////////////
////////////////////////////////////////
auto main(const int argc, const char **argv) -> int {
    using namespace stormkit;

    core::setupSignalHandler();

    auto app = App {};
    return app.run(argc, argv);
}
