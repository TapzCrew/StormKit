// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC DescriptorPool: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Pool;

        struct Size {
            DescriptorType type;
            core::ArraySize descriptor_count;
        };

        DescriptorPool(const Device &device, std::vector<Size> sizes, core::ArraySize max_sets);
        ~DescriptorPool();

        DescriptorPool(DescriptorPool &&);
        DescriptorPool &operator=(DescriptorPool &&);

        std::vector<DescriptorSet> allocateDescriptorSets(core::ArraySize count,
                                                          const DescriptorSetLayout &layout) const;
        inline DescriptorSet allocateDescriptorSet(const DescriptorSetLayout &layout) const;

        std::vector<DescriptorSetOwnedPtr>
            allocateDescriptorSetsPtr(core::ArraySize count,
                                      const DescriptorSetLayout &layout) const;
        inline DescriptorSetOwnedPtr
            allocateDescriptorSetPtr(const DescriptorSetLayout &layout) const;

        inline const Device &device() const noexcept;

        inline vk::DescriptorPool vkDescriptorPool() const noexcept;
        inline operator vk::DescriptorPool() const noexcept;
        inline vk::DescriptorPool vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstPtr m_device;

        RAIIVkDescriptorPool m_vk_descriptor_pool;
    };
} // namespace storm::render

#include "DescriptorPool.inl"
