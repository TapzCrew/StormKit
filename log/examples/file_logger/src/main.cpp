#include <storm/log/FileLogger.hpp>
#include <storm/log/LogHandler.hpp>

#include <storm/core/Types.hpp>

#include <cstdlib>

struct Bar {
    std::string d = "FooBar";
};

CUSTOM_FORMAT(Bar, "Bar { .d = {} }", data.d)

struct Foo {
    storm::core::UInt32 a = 0u;
    float b               = 2.3f;
    Bar c                 = Bar {};
};

CUSTOM_FORMAT(Foo, "Foo { .a = {}, .b = {}, .c = {} }", data.a, data.b, data.c)

int main() {
    using namespace storm;
    using log::operator""_module;

    log::LogHandler::setupLogger<log::FileLogger>(std::filesystem::path { "log/" });

    log::LogHandler::ilog("This is an information");
    log::LogHandler::dlog("This is a debug information");
    log::LogHandler::wlog("This is a warning");
    log::LogHandler::elog("This is an error");
    log::LogHandler::flog("This is a fatal error");

    const auto integer = 5u;
    const auto real    = 8.5f;
    log::LogHandler::ilog("you can format your string like this {1}, {2}", integer, real);

    log::LogHandler::ilog("Foo"_module, "you can specify a module");

    const auto foo = Foo {};
    log::LogHandler::ilog("you can format complexes structures\n{1}", foo);

    return EXIT_SUCCESS;
}
