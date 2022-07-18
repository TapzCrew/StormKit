// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

//#include <storm/render/pipeline/DescriptorSetLayout.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Shader: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Shader_Module;

        Shader(std::filesystem::path filepath, ShaderStageFlag type, const Device &device);
        Shader(core::ByteConstSpan data, ShaderStageFlag type, const Device &device);
        Shader(std::span<const SpirvID> data, ShaderStageFlag type, const Device &device);
        ~Shader();

        Shader(const Shader &) = delete;
        auto operator=(const Shader &) -> Shader & = delete;

        Shader(Shader &&) noexcept;
        auto operator=(Shader &&) noexcept -> Shader &;

        [[nodiscard]] auto type() const noexcept -> ShaderStageFlag;
        [[nodiscard]] auto source() const noexcept -> core::ByteConstSpan;
        // const DescriptorSetLayout &descriptorSetLayout() const noexcept;

        [[nodiscard]] operator VkShaderModule() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkShaderModule;

      private:
        auto compile() noexcept -> void;
        auto reflect() noexcept -> void;

        ShaderStageFlag m_type   = ShaderStageFlag::None;
        core::ByteArray m_source = {};

        VkShaderModule m_shader_module = VK_NULL_HANDLE;
        // DescriptorSetLayout m_descriptor_set_layout;
    };
} // namespace stormkit::gpu

#include "Shader.inl"
