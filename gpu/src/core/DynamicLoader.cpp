// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.gpu.core.dynamicloader;
#else
    /////////// - StormKit::render - ///////////
    #include <stormkit/gpu/core/DynamicLoader.mpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::DynamicLoader(std::filesystem::path vulkan_lib_path)
        : m_loader { vulkan_lib_path } {
        m_success = m_loader.loaded();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::~DynamicLoader() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    DynamicLoader::DynamicLoader(DynamicLoader &&other) noexcept
        : m_success { std::exchange(other.m_success, false) }, m_loader { std::move(
                                                                   other.m_loader) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto DynamicLoader::operator=(DynamicLoader &&other) noexcept -> DynamicLoader & {
        if (&other == this) [[unlikely]]
            return *this;

        m_success = std::exchange(other.m_success, false);
        m_loader  = std::move(other.m_loader);

        return *this;
    }
} // namespace stormkit::gpu
