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
// clang-format on
#else
    /////////// - STL - ///////////
    #include <stdexcept>

    /////////// - StormKit::core - ///////////
    #include <stormkit/core/DynamicLoader.mpp>
    #include <stormkit/core/Format.mpp>
#endif

#include <dlfcn.h>

using namespace stormkit;
using namespace stormkit::core;

namespace stormkit::core {
    namespace details {
        struct DynamicLoaderImpl {
            void *library_handle = nullptr;
        };
    } // namespace details

    ////////////////////////////////////////
    ////////////////////////////////////////
    DynamicLoader::DynamicLoader(std::filesystem::path filepath)
        : m_impl {}, m_filepath { std::move(filepath) } {
        m_impl->library_handle = dlopen(m_filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);

        if (!m_impl->library_handle) {
            auto system_reason = std::string(dlerror());
            throw std::runtime_error { format("Failed to load module \"{}\", reason: {}",
                                              m_filepath.string(),
                                              system_reason) };
        } else
            m_is_loaded = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    DynamicLoader::~DynamicLoader() {
        if (m_impl->library_handle) dlclose(m_impl->library_handle);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::DynamicLoader(DynamicLoader&& other) noexcept
        : m_impl { std::exchange(other.m_impl, {}) }, m_is_loaded { std::exchange(other.m_is_loaded,
                                                                                  false) },
          m_filepath { std::move(other.m_filepath) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DynamicLoader::operator=(DynamicLoader&& other) noexcept -> DynamicLoader& {
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
        auto func =
            dlsym(const_cast<core::Pimpl<details::DynamicLoaderImpl>&>(m_impl)->library_handle,
                  std::data(name));

        if (!func) {
            auto system_reason = std::string(dlerror());
            throw std::runtime_error { format("Failed to load function \"{}()\" (on module "
                                              "\"{}\"), reason: {}",
                                              name,
                                              m_filepath.string(),
                                              system_reason) };
        }

        return func;
    }
} // namespace stormkit::core
