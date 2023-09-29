module;

#include <cstdio>

module stormkit.Core;

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    auto getSTDErr() noexcept -> std::FILE * {
        return stderr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getSTDOut() noexcept -> std::FILE * {
        return stdout;
    }
} // namespace stormkit::core
