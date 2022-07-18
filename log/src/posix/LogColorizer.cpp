// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.details.logcolorizer;

// clang-format off
/////////// - STL - ///////////
import <string_view>;
import <iostream>;

// clang-format on
#else
    /////////// - STL - ///////////
    #include <iostream>
    #include <string_view>

    /////////// - StormKit::log - ///////////
    #include "../LogColorizer.hpp"
#endif

using std::literals::string_view_literals::operator""sv;

namespace stormkit::log::details {
    [[maybe_unused]] static constexpr auto KNRM  = "\x1B[0m"sv;
    [[maybe_unused]] static constexpr auto KRED  = "\x1B[31m"sv;
    [[maybe_unused]] static constexpr auto KGRN  = "\x1B[32m"sv;
    [[maybe_unused]] static constexpr auto KYEL  = "\x1B[33m"sv;
    [[maybe_unused]] static constexpr auto KBLU  = "\x1B[34m"sv;
    [[maybe_unused]] static constexpr auto KMAG  = "\x1B[35m"sv;
    [[maybe_unused]] static constexpr auto KCYN  = "\x1B[36m"sv;
    [[maybe_unused]] static constexpr auto KWHT  = "\x1B[37m"sv;
    [[maybe_unused]] static constexpr auto KGRS  = "\033[1m"sv;
    [[maybe_unused]] static constexpr auto KINV  = "\e[7m"sv;
    [[maybe_unused]] static constexpr auto KBLCK = "\e[30m"sv;

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyBegin(Severity severity, bool to_stderr) noexcept -> void {
        auto &output = (to_stderr) ? std::cerr : std::cout;
        switch (severity) {
            case Severity::Info: output << KBLCK << KINV << KGRN; break;
            case Severity::Warning: output << KBLCK << KINV << KMAG; break;
            case Severity::Error: output << KBLCK << KINV << KYEL; break;
            case Severity::Fatal: output << KBLCK << KINV << KRED; break;
            case Severity::Debug: output << KBLCK << KINV << KCYN; break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto colorifyEnd(bool to_stderr) noexcept -> void {
        auto &output = (to_stderr) ? std::cerr : std::cout;
        output << KNRM;
    }
} // namespace stormkit::log::details
