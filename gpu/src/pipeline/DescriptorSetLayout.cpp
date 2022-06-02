// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.mpp>

#include <stormkit/gpu/pipeline/DescriptorSetLayout.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::DescriptorSetLayout(const Device &device) : DeviceObject { device } {}

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_descriptor_set_layout != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyDescriptorSetLayout(device(), m_descriptor_set_layout, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) noexcept
        : DeviceObject { std::move(other) },
          m_bindings { std::move(other.m_bindings) }, m_hash { std::exchange(other.m_hash, 0) },
          m_descriptor_set_layout { std::exchange(other.m_descriptor_set_layout, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::operator=(DescriptorSetLayout &&other) noexcept
        -> DescriptorSetLayout & {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator  =(std::move(other));
        m_bindings              = std::move(other.m_bindings);
        m_hash                  = std::exchange(other.m_hash, 0);
        m_descriptor_set_layout = std::exchange(other.m_descriptor_set_layout, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::updateHash() noexcept -> void {
        m_hash = std::hash<decltype(m_bindings)> {}(m_bindings);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::bake() -> void {
        const auto &vk = device().table();

        const auto descriptor_set_bindings = [&] {
            auto b = std::vector<VkDescriptorSetLayoutBinding> {};
            b.reserve(std::size(m_bindings));

            std::ranges::transform(m_bindings, std::back_inserter(b), [](const auto &_b) {
                return VkDescriptorSetLayoutBinding {
                    .binding         = _b.binding,
                    .descriptorType  = core::as<VkDescriptorType>(_b.type),
                    .descriptorCount = core::as<core::UInt32>(_b.descriptor_count),
                    .stageFlags      = core::as<VkPipelineStageFlags>(_b.stages)
                };
            });

            return b;
        }();

        const auto create_info = VkDescriptorSetLayoutCreateInfo {
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = core::as<core::UInt32>(std::size(descriptor_set_bindings)),
            .pBindings    = std::data(descriptor_set_bindings)
        };

        CHECK_VK_ERROR(vk.vkCreateDescriptorSetLayout(device(),
                                                      &create_info,
                                                      nullptr,
                                                      &m_descriptor_set_layout));
    }
} // namespace stormkit::gpu

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::DescriptorSetLayout>::operator()(
        const gpu::DescriptorSetLayout &layout) const noexcept -> core::Hash64 {
        return layout.hash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::DescriptorSetLayoutBinding>::operator()(
        const gpu::DescriptorSetLayoutBinding &binding) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, binding.binding);
        core::hashCombine(hash, binding.type);
        core::hashCombine(hash, binding.stages);
        core::hashCombine(hash, binding.descriptor_count);

        return hash;
    }
} // namespace std
