#include <stormkit/Core/Memory.hpp>

#include <stormkit/main/MainMacro.hpp>

import App;

////////////////////////////////////////
////////////////////////////////////////
auto main(const int argc, const char **argv) -> int {
    using namespace stormkit;

    core::setupSignalHandler();

    auto app = App {};
    return app.run(argc, argv);
}
