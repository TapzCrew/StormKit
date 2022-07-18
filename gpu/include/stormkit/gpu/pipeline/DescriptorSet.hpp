// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC DescriptorSet: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Set;

        using Deleter = std::function<void(VkDescriptorSet)>;

        DescriptorSet(const DescriptorPool &pool,
                      std::vector<DescriptorType> type,
                      VkDescriptorSet sets,
                      Deleter deleter,
                      Tag);
        ~DescriptorSet();

        DescriptorSet(const DescriptorSet &) = delete;
        auto operator=(const DescriptorSet &) -> DescriptorSet & = delete;

        DescriptorSet(DescriptorSet &&) noexcept;
        auto operator=(DescriptorSet &&) noexcept -> DescriptorSet &;

        auto update(std::span<const Descriptor> descriptors) -> void;

        [[nodiscard]] auto types() const noexcept -> std::span<const DescriptorType>;

        [[nodiscard]] operator VkDescriptorSet() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkDescriptorSet;

      private:
        const DescriptorPool *m_pool = nullptr;

        std::vector<DescriptorType> m_types;

        VkDescriptorSet m_descriptor_set;

        Deleter m_deleter;
        friend class DescriptorPool;
    };
} // namespace stormkit::gpu

#include "DescriptorSet.inl"
