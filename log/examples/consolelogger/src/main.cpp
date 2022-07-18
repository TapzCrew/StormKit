#ifdef STORMKIT_CXX20_MODULES
import stormkit.core.types;

import stormkit.log.consolelogger;
import stormkit.log.loghandler;
#else
    #include <stormkit/core/Format.hpp>
    #include <stormkit/core/Memory.hpp>
    #include <stormkit/core/Types.hpp>

    #include <stormkit/log/ConsoleLogger.hpp>
    #include <stormkit/log/LogHandler.hpp>
    #include <stormkit/log/Module.hpp>
#endif

#include <stormkit/core/FormatMacro.hpp>

#include <cstdlib>

struct Bar {
    std::string d = "FooBar";
};

CUSTOM_FORMAT(Bar, stormkit::core::format("Bar {{ .d = {} }}", data.d))

struct Foo {
    stormkit::core::UInt32 a = 0u;
    float b                  = 2.3f;
    Bar c                    = Bar {};
};

CUSTOM_FORMAT(Foo,
              stormkit::core::format("Foo {{ .a = {}, .b = {}, .c = {} }}", data.a, data.b, data.c))

////////////////////////////////////////
////////////////////////////////////////
auto main([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv) -> int {
    using namespace stormkit;
    using log::operator""_module;

    core::setupSignalHandler();

    log::LogHandler::setupLogger<log::ConsoleLogger>();

    log::LogHandler::ilog("This is an information");
    log::LogHandler::dlog("This is a debug information");
    log::LogHandler::wlog("This is a warning");
    log::LogHandler::elog("This is an error");
    log::LogHandler::flog("This is a fatal error");

    const auto integer = 5u;
    const auto real    = 8.5f;
    log::LogHandler::ilog("you can format your string like this {}, {}", integer, real);

    log::LogHandler::ilog("Foo"_module, "you can specify a module");

    const auto foo = Foo {};
    log::LogHandler::ilog("you can format complexes structures\n{}", foo);

    return EXIT_SUCCESS;
}
