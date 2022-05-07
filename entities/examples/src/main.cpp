#ifdef STORMKIT_CXX20_MODULES
import stormkit.core.types;

import stormkit.log.loghandler;

import stormkit.entities.system;
import stormkit.entities.entitymanager;
import stormkit.entities.component;
#else
    #include <stormkit/core/Memory.mpp>
    #include <stormkit/core/Types.mpp>

    #include <stormkit/log/LogHandler.mpp>

    #include <stormkit/entities/Component.mpp>
    #include <stormkit/entities/EntityManager.mpp>
    #include <stormkit/entities/System.mpp>
#endif

#include <cstdlib>

auto main() -> int {
    using namespace stormkit;
    using log::operator""_module;

    core::setupSignalHandler();

    log::LogHandler::setupDefaultLogger();

    auto manager = stormkit::entities::EntityManager {};

    log::LogHandler::ilog("Foo"_module, "you can specify a module");

    return EXIT_SUCCESS;
}
