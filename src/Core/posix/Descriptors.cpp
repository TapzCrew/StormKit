module stormkit.Core;

#include <cstdio>

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