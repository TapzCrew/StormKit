// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Shader: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Shader_Module;

        Shader(std::filesystem::path filepath, ShaderStage type, const Device &device);
        Shader(core::span<const core::Byte> data, ShaderStage type, const Device &device);
        Shader(core::span<const SpirvID> data, ShaderStage type, const Device &device);
        ~Shader();

        Shader(Shader &&);
        Shader &operator=(Shader &&);

        inline ShaderStage type() const noexcept;
        inline storm::core::span<const core::Byte> source() const noexcept;
        inline const Device &device() const noexcept;
        inline const DescriptorSetLayout &descriptorSetLayout() const noexcept;

        inline vk::ShaderModule vkShaderModule() const noexcept;
        inline operator vk::ShaderModule() const noexcept;
        inline vk::ShaderModule vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        void compile() noexcept;
        void reflect() noexcept;

        DeviceConstPtr m_device;

        ShaderStage m_type;
        std::vector<core::Byte> m_source;

        RAIIVkShaderModule m_vk_shader_module;
        DescriptorSetLayout m_descriptor_set_layout;
    };
} // namespace storm::render

#include "Shader.inl"
