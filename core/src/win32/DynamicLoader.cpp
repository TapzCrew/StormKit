// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.core.dynamicloader;

// clang-format off
/////////// - STL - ///////////
import <stdexcept>

/////////// - StormKit::core - ///////////
import stormkit.core.format;
import stormkit.core.strings;
// clang-format on
#else
    /////////// - STL - ///////////
    #include <stdexcept>

    /////////// - StormKit::core - ///////////
    #include <stormkit/core/DynamicLoader.mpp>
    #include <stormkit/core/Format.mpp>
    #include <stormkit/core/Strings.mpp>
#endif

/////////// - Win32API - ///////////
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
DynamicLoader::DynamicLoader(DynamicLoader &&other) = default;

/////////////////////////////////////
/////////////////////////////////////
auto DynamicLoader::operator=(DynamicLoader &&) -> DynamicLoader & = default;

////////////////////////////////////////
////////////////////////////////////////
auto DynamicLoader::doGetFunc(std::string_view name) const -> void * {
    return reinterpret_cast<void *>(GetProcAddress(m_impl->library_handle, name.data()));
}
