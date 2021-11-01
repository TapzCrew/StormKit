// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::DescriptorSetLayout(const Device &device) : m_device { &device } {
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::~DescriptorSetLayout() = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&rhs) noexcept
    : m_device { rhs.m_device }, m_bindings { std::move(rhs.m_bindings) }, m_hash { rhs.m_hash },
      m_vk_descriptor_set_layout { std::move(rhs.m_vk_descriptor_set_layout) } {
    rhs.m_vk_descriptor_set_layout.release();
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout &DescriptorSetLayout::operator=(DescriptorSetLayout &&rhs) {
    if (&rhs == this) return *this;

    m_device                   = rhs.m_device;
    m_bindings                 = std::move(rhs.m_bindings);
    m_hash                     = rhs.m_hash;
    m_vk_descriptor_set_layout = std::move(rhs.m_vk_descriptor_set_layout);

    rhs.m_vk_descriptor_set_layout.release();

    return *this;
}

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSetLayout::updateHash() noexcept {
    m_hash = std::hash<decltype(m_bindings)> {}(m_bindings);
}

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSetLayout::bake() {
    auto descriptor_set_bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
    descriptor_set_bindings.reserve(std::size(m_bindings));

    for (const auto &descriptor_set_binding : m_bindings) {
        const auto vk_descriptor_set_binding =
            vk::DescriptorSetLayoutBinding {}
                .setBinding(descriptor_set_binding.binding)
                .setDescriptorType(toVK(descriptor_set_binding.type))
                .setDescriptorCount(
                    gsl::narrow_cast<core::UInt32>(descriptor_set_binding.descriptor_count))
                .setStageFlags(toVK(descriptor_set_binding.stages));

        descriptor_set_bindings.emplace_back(std::move(vk_descriptor_set_binding));
    }

    const auto descriptor_layout =
        vk::DescriptorSetLayoutCreateInfo {}
            .setBindingCount(gsl::narrow_cast<core::UInt32>(std::size(descriptor_set_bindings)))
            .setPBindings(std::data(descriptor_set_bindings));

    m_vk_descriptor_set_layout = m_device->createVkDescriptorSetLayout(descriptor_layout);
}

namespace std {
    core::Hash64 hash<storm::render::DescriptorSetLayout>::operator()(
        const storm::render::DescriptorSetLayout &layout) const noexcept {
        return layout.hash();
    }

    core::Hash64 hash<storm::render::DescriptorSetLayoutBinding>::operator()(
        const storm::render::DescriptorSetLayoutBinding &binding) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, binding.binding);
        core::hashCombine(hash, binding.type);
        core::hashCombine(hash, binding.stages);
        core::hashCombine(hash, binding.descriptor_count);

        return hash;
    }

    core::Hash64 hash<storm::render::DescriptorSetLayoutBindingArray>::operator()(
        const storm::render::DescriptorSetLayoutBindingArray &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::DescriptorSetLayoutBindingSpan>::operator()(
        const storm::render::DescriptorSetLayoutBindingSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::DescriptorSetLayoutBindingConstSpan>::operator()(
        const storm::render::DescriptorSetLayoutBindingConstSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }
} // namespace std
