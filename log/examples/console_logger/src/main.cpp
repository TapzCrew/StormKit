#include <storm/log/LogHandler.hpp>

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
    using namespace storm::log;

    /* LogHandler::setupDefaultLogger();

     LogHandler::ilog("This is information");
     LogHandler::dlog("This is debug");
     LogHandler::wlog("This is warning");
     LogHandler::elog("This is error");
     LogHandler::flog("This is fatal error");

     const auto integer = 5u;
     const auto real    = 8.5f;
     LogHandler::ilog("you can format your string like this {1}, {2}", integer,
     real);

     LogHandler::ilog("Foo"_module, "you can specify a module");

     const auto foo = Foo{};
     LogHandler::ilog("you can format complexes structures\n{1}", foo);*/

    return EXIT_SUCCESS;
}
