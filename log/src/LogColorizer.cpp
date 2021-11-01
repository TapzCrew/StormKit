// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/LogColorizer.hpp>

#include <storm/core/Platform.hpp>

using namespace storm;
using namespace storm::log;

#if defined(STORMKIT_OS_WINDOWS)
    #include <Windows.h>

[[maybe_unused]] static constexpr core::UInt8 KBLA = 0;
[[maybe_unused]] static constexpr core::UInt8 KRED = 12;
[[maybe_unused]] static constexpr core::UInt8 KGRN = 2;
[[maybe_unused]] static constexpr core::UInt8 KYEL = 14;
[[maybe_unused]] static constexpr core::UInt8 KBLU = 9;
[[maybe_unused]] static constexpr core::UInt8 KMAG = 13;
[[maybe_unused]] static constexpr core::UInt8 KCYN = 11;
[[maybe_unused]] static constexpr core::UInt8 KWHT = 15;
[[maybe_unused]] static constexpr core::UInt8 KGRS = 8;

namespace storm::log {
    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin(Severity severity, bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

        core::UInt8 background = KBLA;
        core::UInt8 text       = KWHT;
        switch (severity) {
            case Severity::Info:
                background = KGRN;
                text       = KBLA;
                break;
            case Severity::Warning:
                background = KMAG;
                text       = KBLA;
                break;
            case Severity::Error:
                background = KYEL;
                text       = KBLA;
                break;
            case Severity::Fatal:
                background = KRED;
                text       = KBLA;
                break;
            case Severity::Debug:
                background = KCYN;
                text       = KBLA;
                break;
        }

        SetConsoleTextAttribute(handle, (background << 4) + text);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyEnd(bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, (KBLA << 4) + KWHT);
    }
} // namespace storm::log
#elif defined(STORMKIT_OS_IOS)
namespace storm::log {
    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin([[maybe_unused]] Severity severity, [[maybe_unused]] bool to_stderr) {}

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBEnd([[maybe_unused]] bool to_stderr) {}
} // namespace storm::log
#elif defined(STORMKIT_POSIX)
using std::literals::string_view_literals::operator""sv;

namespace storm::log {
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
    void colorifyBegin(Severity severity, bool to_stderr) noexcept {
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
    void colorifyEnd(bool to_stderr) noexcept {
        auto &output = (to_stderr) ? std::cerr : std::cout;
        output << KNRM;
    }
} // namespace storm::log
#endif
