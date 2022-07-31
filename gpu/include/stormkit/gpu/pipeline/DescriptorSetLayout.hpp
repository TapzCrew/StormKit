// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    struct DescriptorSetLayoutBinding {
        core::UInt32 binding;
        DescriptorType type;
        ShaderStageFlag stages;
        core::USize descriptor_count;
    };

    class STORMKIT_PUBLIC DescriptorSetLayout: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Set_Layout;

        explicit DescriptorSetLayout(const Device &device);
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout &) = delete;
        auto operator=(const DescriptorSetLayout &) -> DescriptorSetLayout & = delete;

        DescriptorSetLayout(DescriptorSetLayout &&) noexcept;
        auto operator=(DescriptorSetLayout &&) noexcept -> DescriptorSetLayout &;

        auto addBinding(const DescriptorSetLayoutBinding &binding) -> void;

        auto bake() -> void;

        [[nodiscard]] auto hash() const noexcept -> core::Hash64;
        [[nodiscard]] auto bindings() const noexcept -> std::span<const DescriptorSetLayoutBinding>;

        [[nodiscard]] operator VkDescriptorSetLayout() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkDescriptorSetLayout;

        [[nodiscard]] auto operator==(const DescriptorSetLayout &second) const noexcept -> bool;

      private:
        auto updateHash() noexcept -> void;

        std::vector<DescriptorSetLayoutBinding> m_bindings = {};

        core::Hash64 m_hash = 0;

        VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

HASH_FUNC(stormkit::gpu::DescriptorSetLayout)
HASH_FUNC(stormkit::gpu::DescriptorSetLayoutBinding)

#include "DescriptorSetLayout.inl"