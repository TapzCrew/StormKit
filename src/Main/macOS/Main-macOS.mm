#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#include <span>
#include <string_view>
#include <vector>

extern auto userMain(std::span<const std::string_view>) -> int;

auto main(const int argc, const char **argv) -> int {
    stormkit::core::backtraceInit(argv[0]);

    [[NSFileManager defaultManager]
        changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

    auto args = std::vector<std::string_view> {};

    for (auto i = 0u; i < argc; ++i) args.emplace_back(argv[i]);

    return userMain(args);
}
