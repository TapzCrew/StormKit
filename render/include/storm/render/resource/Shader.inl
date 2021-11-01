// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    ShaderStage Shader::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<const core::Byte> Shader::source() const noexcept { return m_source; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Device &Shader::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const DescriptorSetLayout &Shader::descriptorSetLayout() const noexcept {
        return m_descriptor_set_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::ShaderModule Shader::vkShaderModule() const noexcept {
        STORMKIT_EXPECTS(m_vk_shader_module);
        return *m_vk_shader_module;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::operator vk::ShaderModule() const noexcept {
        STORMKIT_EXPECTS(m_vk_shader_module);
        return *m_vk_shader_module;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::ShaderModule Shader::vkHandle() const noexcept { return vkShaderModule(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Shader::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkShaderModule_T *());
    }
} // namespace storm::render
