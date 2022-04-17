#ifdef STORMKIT_CXX20_MODULES
import stormkit.log.filelogger;
import stormkit.log.loghandler;
#else
    #include <stormkit/core/Format.mpp>
    #include <stormkit/core/Types.mpp>

    #include <stormkit/log/FileLogger.mpp>
    #include <stormkit/log/LogHandler.mpp>
    #include <stormkit/log/Module.mpp>
#endif

#include <stormkit/core/FormatMacro.hpp>

#include <stormkit/main/MainMacro.hpp>

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

int main() {
    using namespace stormkit;
    using log::operator""_module;

    log::LogHandler::setupLogger<log::FileLogger>(std::filesystem::path { "log/" });

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
