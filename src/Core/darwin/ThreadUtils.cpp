module;

#include "ThreadUtilsImpl.h"

module stormkit.Core;

import std;

import :Parallelism.ThreadUtils;

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setCurrentThreadName(std::string_view name) -> void {
        setCurrentThreadName(std::data(name));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName([[maybe_unused]] std::thread& thread, [[maybe_unused]] std::string_view name)
        -> void {
        // auto id = thread.native_handle();
        // details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName([[maybe_unused]] std::jthread   & thread,
                       [[maybe_unused]] std::string_view name) -> void {
        // auto id = thread.native_handle();
        // details::setThreadName(id, name);
    }
} // namespace stormkit::core
