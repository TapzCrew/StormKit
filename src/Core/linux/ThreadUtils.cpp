module;

#include <pthread.h>

module stormkit.Core;

import std;

namespace stormkit::core {
    namespace details {
        ////////////////////////////////////////
        ////////////////////////////////////////
        auto setThreadName(pthread_t id, std::string_view name) noexcept -> void {
            pthread_setname_np(id, std::data(name));
        }
    } // namespace details

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setCurrentThreadName(std::string_view name) noexcept -> void {
        const auto id = pthread_self();
        details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName(std::thread& thread, std::string_view name) noexcept -> void {
        const auto id = thread.native_handle();
        details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName(std::jthread& thread, std::string_view name) noexcept -> void {
        const auto id = thread.native_handle();
        details::setThreadName(id, name);
    }
} // namespace stormkit::core
