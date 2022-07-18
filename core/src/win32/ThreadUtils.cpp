
#include <stormkit/core/ThreadUtils.hpp>

#include <windows.h>

constexpr auto MS_VC_EXCEPTION = DWORD { 0x406D1388 };

#pragma pack(push, 8)
struct ThreadNameInfo {
    DWORD dwType = 0x1000;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags = 0;
};
#pragma pack(pop)

namespace stormkit::core {
    namespace details {
        ////////////////////////////////////////
        ////////////////////////////////////////
        auto setThreadName(DWORD id, std::string_view name) -> void {
            auto info = ThreadNameInfo { .szName = std::data(name), .dwThreadID = id };

            __try {
                RaiseException(MS_VC_EXCEPTION,
                               0,
                               sizeof(info) / sizeof(ULONG_PTR),
                               reinterpret_cast<ULONG_PTR *>(&info));
            } __except (EXCEPTION_EXECUTE_HANDLER) {}
        }
    } // namespace details

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setCurrentThreadName(std::string_view name) -> void {
        auto id = ::GetCurrentThreadId();
        details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName(std::thread &thread, std::string_view name) -> void {
        const auto id = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
        details::setThreadName(id, name);
    }
} // namespace stormkit::core
