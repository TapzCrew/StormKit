// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Shader.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Shader::type() const noexcept -> ShaderStageFlag {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_shader_module);

        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Shader::source() const noexcept -> core::ByteConstSpan {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_shader_module);

        return m_source;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    /*inline auto Shader::descriptorSetLayout() const noexcept -> const DescriptorSetLayout & {
        return m_descriptor_set_layout;
    }*/

    /////////////////////////////////////
    /////////////////////////////////////
    inline Shader::operator VkShaderModule() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Shader::vkHandle() const noexcept -> VkShaderModule {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_shader_module);

        return m_shader_module;
    }
} // namespace stormkit::gpu
