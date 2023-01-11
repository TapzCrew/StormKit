// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Core;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::DynamicLoader(std::filesystem::path vulkan_lib_path) : m_success { true } {
        auto result = core::DynamicLoader::load(DEFAULT_VULKAN_LIB);

        if (not result) m_success = false;
        else
            m_loader = std::move(*result);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::~DynamicLoader() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::DynamicLoader(DynamicLoader&& other) noexcept
        : m_success { std::exchange(other.m_success, false) },
          m_loader { std::move(other.m_loader) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DynamicLoader::operator=(DynamicLoader&& other) noexcept -> DynamicLoader& {
        if (&other == this) [[unlikely]]
            return *this;

        m_success = std::exchange(other.m_success, false);
        m_loader  = std::move(other.m_loader);

        return *this;
    }
} // namespace stormkit::gpu
