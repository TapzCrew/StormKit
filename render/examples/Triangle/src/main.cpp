#include <stormkit/log/LogHandler.mpp>

#include <stormkit/main/MainMacro.hpp>

#include "App.mpp"

auto main(const int argc, const char **argv) -> int {
    using namespace stormkit;

    log::LogHandler::setupDefaultLogger();

    auto app = App {};
    return app.run(argc, argv);
}
