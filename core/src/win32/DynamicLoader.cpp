// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stdexcept>

#include <stormkit/core/DynamicLoader.hpp>
#include <stormkit/core/Format.hpp>
#include <stormkit/core/Strings.hpp>


#include <windows.h>

using namespace stormkit;
using namespace stormkit::core;

namespace stormkit::core::details {
    struct DynamicLoaderImpl {
        HMODULE library_handle = nullptr;
    };
} // namespace stormkit::core::details

////////////////////////////////////////
////////////////////////////////////////
DynamicLoader::DynamicLoader(std::filesystem::path filepath) : m_filepath { std::move(filepath) } {
    m_impl->library_handle = LoadLibraryW(m_filepath.c_str());

    if (!m_impl->library_handle) {
        auto buffer = ZString { nullptr };

        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                           FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr,
                       GetLastError(),
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       buffer,
                       0,
                       nullptr);

        auto system_reason = std::string { buffer };
        throw std::runtime_error {
            format("Failed to load module \"{}\", reason: {}", m_filepath.string(), system_reason)
        };
    } else
        m_is_loaded = true;
}

////////////////////////////////////////
////////////////////////////////////////
DynamicLoader::~DynamicLoader() {
    if (m_impl->library_handle) FreeLibrary(m_impl->library_handle);
}

/////////////////////////////////////
/////////////////////////////////////
DynamicLoader::DynamicLoader(DynamicLoader &&other) noexcept
    : m_impl { std::exchange(other.m_impl, {}) }, m_is_loaded { std::exchange(other.m_is_loaded,
                                                                              false) },
      m_filepath { std::move(other.m_filepath) } {
}

/////////////////////////////////////
/////////////////////////////////////
auto DynamicLoader::operator=(DynamicLoader &&other) noexcept -> DynamicLoader & {
    if (&other == this) [[unlikely]]
        return *this;

    m_impl      = std::exchange(other.m_impl, {});
    m_is_loaded = std::exchange(other.m_is_loaded, false);
    m_filepath  = std::move(other.m_filepath);

    return *this;
}

////////////////////////////////////////
////////////////////////////////////////
auto DynamicLoader::doGetFunc(std::string_view name) const -> void * {
    return reinterpret_cast<void *>(GetProcAddress(m_impl->library_handle, name.data()));
}
