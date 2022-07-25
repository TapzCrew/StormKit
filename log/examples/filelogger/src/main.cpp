#ifdef STORMKIT_CXX20_MODULES
import stormkit.log.filelogger;
import stormkit.log.loghandler;
#else
    #include <stormkit/core/Format.hpp>
    #include <stormkit/core/Memory.hpp>
    #include <stormkit/core/Types.hpp>

    #include <stormkit/log/FileLogger.hpp>
    #include <stormkit/log/Logger.hpp>
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

    auto logger =
        log::Logger::createLoggerInstance<log::FileLogger>(std::filesystem::path { "log/" });

    log::Logger::ilog("This is an information");
    log::Logger::dlog("This is a debug information");
    log::Logger::wlog("This is a warning");
    log::Logger::elog("This is an error");
    log::Logger::flog("This is a fatal error");

    const auto integer = 5u;
    const auto real    = 8.5f;
    log::Logger::ilog("you can format your string like this {}, {}", integer, real);

    log::Logger::ilog("Foo"_module, "you can specify a module");

    const auto foo = Foo {};
    log::Logger::ilog("you can format complexes structures\n{}", foo);

    return EXIT_SUCCESS;
}
