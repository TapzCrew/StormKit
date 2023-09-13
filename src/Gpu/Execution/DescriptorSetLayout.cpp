// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Execution.Descriptors;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::DescriptorSetLayout(const Device& device) : DeviceObject { device } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::~DescriptorSetLayout() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
        -> DescriptorSetLayout& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::updateHash() noexcept -> void {
        m_hash = core::hashCombineRange(m_bindings);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSetLayout::bake() -> void {
        const auto vk_descriptor_set_bindings =
            m_bindings | std::views::transform([](auto&& binding) {
                return vk::DescriptorSetLayoutBinding {}
                    .setBinding(binding.binding)
                    .setDescriptorType(core::as < vk::DescriptorType(binding.type))
                    .setDescriptorCount(binding.descriptor_count)
                    .setStages(core::as<vk::PipelineStageFlagsBits>(binding.stages));
            }) |
            std::ranges::to<std::vector>();

        const auto create_info =
            vk::DescriptorSetLayoutCreateInfo {}.setBindings(vk_descriptor_set_bindings);

        try {
            m_vk_descriptor_set_layout =
                vk::raii::DescriptorSetLayout { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }
} // namespace stormkit::gpu
