module;

#define WIN32_LEAN_AND_MEAN
#define __SPECSTRINGS_STRICT_LEVEL 0
#include <windows.h>

module stormkit.Core;

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    auto getSTDErr() noexcept -> std::FILE * {
        return std::bit_cast<std::FILE *>(GetStdHandle(STD_ERROR_HANDLE));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getSTDOut() noexcept -> std::FILE * {
        return std::bit_cast<std::FILE *>(GetStdHandle(STD_OUTPUT_HANDLE));
    }
} // namespace stormkit::core