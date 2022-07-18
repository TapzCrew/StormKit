// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/DescriptorSet.hpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC DescriptorPool: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Pool;

        struct Size {
            DescriptorType type;
            core::UInt32 descriptor_count;
        };

        DescriptorPool(const Device &device, std::span<const Size> sizes, core::UInt32 max_sets);
        ~DescriptorPool();

        DescriptorPool(const DescriptorPool &) = delete;
        auto operator=(const DescriptorPool &) -> DescriptorPool & = delete;

        DescriptorPool(DescriptorPool &&) noexcept;
        auto operator=(DescriptorPool &&) noexcept -> DescriptorPool &;

        [[nodiscard]] auto createDescriptorSet(const DescriptorSetLayout &layout) const
            -> DescriptorSet;
        [[nodiscard]] auto allocateDescriptorSet(const DescriptorSetLayout &layout) const
            -> DescriptorSetOwnedPtr;
        [[nodiscard]] auto allocateRefCountedDescriptorSet(const DescriptorSetLayout &layout) const
            -> DescriptorSetSharedPtr;

        [[nodiscard]] auto createDescriptorSets(core::USize count,
                                                const DescriptorSetLayout &layout) const
            -> std::vector<DescriptorSet>;
        [[nodiscard]] auto allocateDescriptorSets(core::USize count,
                                                  const DescriptorSetLayout &layout) const
            -> std::vector<DescriptorSetOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedDescriptorSets(core::USize count,
                                                            const DescriptorSetLayout &layout) const
            -> std::vector<DescriptorSetSharedPtr>;

        [[nodiscard]] operator VkDescriptorPool() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkDescriptorPool;

      private:
        auto createVkDescriptorSets(core::USize count, const DescriptorSetLayout &layout) const
            -> std::pair<std::vector<VkDescriptorSet>, std::vector<DescriptorType>>;
        auto deleteDescriptorSet(VkDescriptorSet set) const -> void;

        VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "DescriptorPool.inl"
