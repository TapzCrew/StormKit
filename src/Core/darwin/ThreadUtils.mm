#import <Foundation/NSThread.h>
#import <Foundation/NSString.h>

#include <mach/mach.h>

#include <string_view>
#include <thread>

namespace stormkit::core {
    namespace details {
        ////////////////////////////////////////
        ////////////////////////////////////////
        auto setThreadName(NSThread *id, std::string_view name) -> void {
            [id setName: [NSString stringWithCString: std::data(name)]];
        }
    } // namespace details

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setCurrentThreadName(std::string_view name) -> void {
        auto id = [NSThread currentThread];
        details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName(std::thread& thread, std::string_view name) -> void {
        //auto id = thread.native_handle();
        //details::setThreadName(id, name);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto setThreadName(std::jthread& thread, std::string_view name) -> void {
        //auto id = thread.native_handle();
        //details::setThreadName(id, name);
    }
} // namespace stormkit::core
