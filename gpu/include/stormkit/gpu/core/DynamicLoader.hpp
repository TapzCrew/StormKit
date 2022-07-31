// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>


#include <filesystem>
#include <string_view>


#include <stormkit/core/DynamicLoader.hpp>


#include <stormkit/gpu/core/Vulkan.hpp>

namespace stormkit::gpu {
#ifdef STORMKIT_OS_LINUX
    static constexpr auto DEFAULT_VULKAN_LIB = std::string_view { "libvulkan.so" };
#elif defined(STORMKIT_OS_WINDOWS)
    static constexpr auto DEFAULT_VULKAN_LIB = std::string_view { "vulkan-1.dll" };
#elif defined(STORMKIT_OS_APPLE)
    static constexpr auto DEFAULT_VULKAN_LIB = std::string_view { "libvulkan.dylib" };
#endif

    class STORMKIT_PUBLIC DynamicLoader {
      public:
        using GetInstanceProcAddrType = PFN_vkVoidFunction(VkInstance, const char *);

        explicit DynamicLoader(std::filesystem::path vulkan_lib_path = DEFAULT_VULKAN_LIB);
        ~DynamicLoader();

        DynamicLoader(const DynamicLoader &) = delete;
        auto operator=(const DynamicLoader &) -> DynamicLoader & = delete;

        DynamicLoader(DynamicLoader &&) noexcept;
        auto operator=(DynamicLoader &&) noexcept -> DynamicLoader &;

        auto success() const noexcept -> bool;

        template<typename T>
        auto get(std::string_view name) const noexcept -> T;

      private:
        bool m_success = false;
        core::DynamicLoader m_loader;
    };
} // namespace stormkit::gpu

#include "DynamicLoader.inl"