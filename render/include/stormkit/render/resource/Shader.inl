// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
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

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Shader::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_SHADER_MODULE;
    }
} // namespace stormkit::render
