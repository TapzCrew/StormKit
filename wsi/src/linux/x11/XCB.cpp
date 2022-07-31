// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <iostream>
#include <memory>

/////////// - StormKit::core - ///////////
#include <stormkit/core/HashMap.hpp>
#include <stormkit/core/Strings.hpp>

/////////// - StormKit::wsi - ///////////
#include "Log.hpp"
#include "XCB.hpp"

/////////// - StormKit::core - ///////////
#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/MemoryMacro.hpp>

#define STORMKIT_XCB_SCOPED(name, type, var, global, deleter)                     \
    name##Scoped::name##Scoped(type *handle) noexcept : m_handle { handle } {     \
        if (m_handle != nullptr) var++;                                           \
    }                                                                             \
                                                                                  \
    name##Scoped::~name##Scoped() noexcept {                                      \
        if (m_handle != nullptr) {                                                \
            var--;                                                                \
                                                                                  \
            if (var == 0) {                                                       \
                deleter(m_handle);                                                \
                m_handle = nullptr;                                               \
                global   = nullptr;                                               \
            }                                                                     \
        }                                                                         \
    }                                                                             \
                                                                                  \
    name##Scoped::name##Scoped(name##Scoped &&other) noexcept                     \
        : m_handle { std::exchange(other.m_handle, nullptr) } {}                  \
                                                                                  \
    auto name##Scoped::operator=(name##Scoped &&other) noexcept->name##Scoped & { \
        if (this == &other) return *this;                                         \
                                                                                  \
        m_handle = std::exchange(other.m_handle, nullptr);                        \
                                                                                  \
        return *this;                                                             \
    }

namespace stormkit::wsi::details::x11 {
    using XCBConnectionPtr   = xcb_connection_t *;
    using XCBErrorContextPtr = xcb_errors_context_t *;
    using XCBErrorPtr        = xcb_generic_error_t *;
    STORMKIT_RAII_CAPSULE(XCBGenericError, xcb_generic_error_t, std::free)

    auto xcb_connection    = XCBConnectionPtr { nullptr };
    auto xcb_error_context = XCBErrorContextPtr { nullptr };

    auto xcb_connection_reference_count    = 0u;
    auto xcb_error_context_reference_count = 0u;

    auto xcb_atoms = stormkit::core::StringHashMap<xcb_atom_t> {};

    STORMKIT_XCB_SCOPED(XCBConnection,
                        xcb_connection_t,
                        xcb_connection_reference_count,
                        xcb_connection,
                        xcb_disconnect)
    STORMKIT_XCB_SCOPED(XCBErrorContext,
                        xcb_errors_context_t,
                        xcb_error_context_reference_count,
                        xcb_error_context,
                        xcb_errors_context_free)

    /////////////////////////////////////
    /////////////////////////////////////
    auto getXCBConnection() noexcept -> XCBConnectionScoped {
        if (!xcb_connection) [[unlikely]] {
            xcb_connection = xcb_connect(nullptr, nullptr);
            dlog("New xcb connection {}", core::as<void *>(xcb_connection));
        }

        STORMKIT_EXPECTS(xcb_connection);
        return xcb_connection;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getXCBAtom(std::string_view name, bool only_if_exists) noexcept
        -> core::Expected<xcb_atom_t, Error> {
        STORMKIT_RAII_CAPSULE(Reply, xcb_intern_atom_reply_t, std::free)

        auto it = xcb_atoms.find(name);

        if (it != std::ranges::end(xcb_atoms)) return it->second;

        auto connection = getXCBConnection();

        auto cookie =
            xcb_intern_atom(connection, (only_if_exists) ? 1 : 0, std::size(name), std::data(name));

        auto error = XCBErrorPtr {};
        auto reply = ReplyScoped { xcb_intern_atom_reply(connection, cookie, &error) };

        if (error || !reply)
            return core::Unexpected { Error { (error) ? getXCBError(error)
                                                      : std::string { " " } } };

        auto atom = reply->atom;
        xcb_atoms.emplace(name, atom);

        return atom;
    }

    auto getXCBAtomName(xcb_atom_t atom) -> core::Expected<std::string, Error> {
        STORMKIT_RAII_CAPSULE(Reply, xcb_get_atom_name_reply_t, free)

        auto connection = getXCBConnection();

        auto error  = XCBErrorPtr {};
        auto cookie = xcb_get_atom_name(connection, atom);
        auto reply  = ReplyScoped { xcb_get_atom_name_reply(connection, cookie, &error) };

        if (error) return core::Unexpected { Error { getXCBError(error) } };

        return std::string { xcb_get_atom_name_name(reply.get()),
                             core::as<std::size_t>(xcb_get_atom_name_name_length(reply.get())) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getXCBErrorContext() noexcept -> XCBErrorContextScoped {
        if (!xcb_error_context) [[unlikely]] {
            xcb_errors_context_new(getXCBConnection(), &xcb_error_context);
            dlog("New xcb error context {}", core::as<void *>(xcb_error_context));
        }

        STORMKIT_EXPECTS(xcb_error_context);
        return xcb_error_context;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getXCBError(XCBErrorPtr error) -> std::string {
        STORMKIT_EXPECTS(error);

        auto guard = XCBGenericErrorScoped { error };

        auto ctx = getXCBErrorContext();

        const auto major = xcb_errors_get_name_for_major_code(ctx, error->major_code);
        const auto minor =
            xcb_errors_get_name_for_minor_code(ctx, error->major_code, error->minor_code);

        using CStr            = const char *;
        const auto *extension = CStr { nullptr };
        const auto str_error  = xcb_errors_get_name_for_error(ctx, error->major_code, &extension);

        return core::format("{} extension: {} major: {} minor: {}\n",
                            str_error,
                            (extension) ? extension : " ",
                            major,
                            (minor) ? minor : " ");
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto getXIDeviceInfo(xcb_input_device_id_t device_id)
        -> core::Expected<xcb_input_xi_device_info_t *, Error> {
        STORMKIT_RAII_CAPSULE(Reply, xcb_input_xi_query_device_reply_t, free)

        auto connection = getXCBConnection();

        auto error  = XCBErrorPtr {};
        auto cookie = xcb_input_xi_query_device(connection, device_id);
        auto reply  = ReplyScoped { xcb_input_xi_query_device_reply(connection, cookie, &error) };

        if (error) return core::Unexpected { Error { getXCBError(error) } };

        auto device_info_it = xcb_input_xi_query_device_infos_iterator(reply.get());
        for (; device_info_it.rem; xcb_input_xi_device_info_next(&device_info_it)) {
            auto info = device_info_it.data;

            if (info->deviceid == device_id) return info;
        }

        return core::Unexpected { Error { "Device ID not found" } };
    }
} // namespace stormkit::wsi::details::x11
