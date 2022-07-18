// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#pragma once

/////////// - XCB - ///////////
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_errors.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xinput.h>

#define STORMKIT_XCB_SCOPED(name, type)                                  \
    struct name##Scoped {                                                \
        name##Scoped(type *handle) noexcept;                             \
        ~name##Scoped() noexcept;                                        \
                                                                         \
        name##Scoped(const name##Scoped &) = delete;                     \
        auto operator=(const name##Scoped &) -> name##Scoped & = delete; \
                                                                         \
        name##Scoped(name##Scoped &&) noexcept;                          \
        auto operator=(name##Scoped &&) noexcept -> name##Scoped &;      \
                                                                         \
        operator type *() const noexcept { return m_handle; }            \
                                                                         \
      private:                                                           \
        type *m_handle = nullptr;                                        \
    }

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.details.x11.xcb;

// clang-format off
/////////// - StormKit::core - ///////////
import stormkit.core.expected;

/////////// - STL - ///////////
import <string>;
import <string_view>;
// clang-format on
export {
#else
    /////////// - StormKit::core - ///////////
    #include <stormkit/core/Expected.mpp>

    /////////// - STL - ///////////
    #include <string>
    #include <string_view>
#endif

    namespace stormkit::wsi::details::x11 {
        using XCBErrorPtr = xcb_generic_error_t *;

        STORMKIT_XCB_SCOPED(XCBConnection, xcb_connection_t);
        STORMKIT_XCB_SCOPED(XCBErrorContext, xcb_errors_context_t);

        struct Error {
            std::string message;
        };

        auto getXCBConnection() noexcept -> XCBConnectionScoped;
        auto getXCBAtom(std::string_view name, bool only_if_exists) noexcept
            -> core::Expected<xcb_atom_t, Error>;
        auto getXCBAtomName(xcb_atom_t atom) -> core::Expected<std::string, Error>;

        auto getXCBErrorContext() noexcept -> XCBErrorContextScoped;
        auto getXCBError(XCBErrorPtr error) -> std::string;

        auto getXIDeviceInfo(xcb_input_device_id_t device_id)
            -> core::Expected<xcb_input_xi_device_info_t *, Error>;
    } // namespace stormkit::wsi::details::x11

#if defined(STORMKIT_CXX20_MODULES)
}
#endif

#undef STORMKIT_XCB_SCOPED
