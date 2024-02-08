// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::core {
    auto printStacktrace();
#if defined(BACKTRACE_SUPPORTED)
    auto backtraceInit(const char* filename) noexcept -> bool;
#endif
} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::core {
#if defined(BACKTRACE_SUPPORTED)
    namespace {
        using BacktraceState         = backtrace_state*;
        auto              __bt_state = BacktraceState { nullptr };
        thread_local auto __counter  = 0;

        extern "C" auto bt_callback(void*,
                                    std::uintptr_t,
                                    const char* filename,
                                    int         line,
                                    const char* function) noexcept -> int {
            // auto status    = 0;
            // auto func_name = "";
            // if (function) func_name = function;
            //
            // const auto demangled = abi::__cxa_demangle(func_name, nullptr, nullptr, &status);
            //
            // if (status != 0) func_name = demangled;
            //
            // auto file_name = "";
            // if (filename) file_name = filename;
            //
            // std::println(stderr, "{}# {} at {}:{}", __counter++, func_name, file_name, line);
            return 0;
        }

        extern "C" auto
            bt_error_callback(void* status, const char* message, int error) noexcept -> void {
            if (status) *std::bit_cast<bool*>(status) = false;

            std::println(stderr, "Failed to print stacktrace, reason: {} code: {}", message, error);
        }
    } // namespace
#endif
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto printStacktrace() {
        std::println(stderr, "================= CALLSTACK =================");
#if defined(STD_STACKTRACE_SUPPORTED)
        const auto st = std::stacktrace::current();
        std::println(stderr, "{}", std::to_string(st));
#elif defined(BACKTRACE_SUPPORTED)
        backtrace_full(__bt_state, 0, bt_callback, bt_error_callback, nullptr);
#endif
    }

#if defined(BACKTRACE_SUPPORTED)
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto backtraceInit(const char* filename) noexcept -> bool {
    #ifdef BACKTRACE_SUPPORTED
        auto status = true;

        __bt_state = backtrace_create_state(filename,
                                            BACKTRACE_SUPPORTS_THREADS,
                                            bt_error_callback,
                                            &status);
        if (__bt_state and status)
            std::println("libbacktrace initialized for {} {}",
                         std::bit_cast<std::intptr_t>(__bt_state),
                         filename);
        else
            std::println(stderr, "failed to initialize libbacktrace");

        return status;
    #else
        std::println("libbacktrace not supported");
        return false;
    #endif
    }
#endif
} // namespace stormkit::core
