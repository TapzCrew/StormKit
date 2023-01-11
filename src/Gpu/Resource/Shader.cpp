// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Core;

import :Resource.Shader;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(std::filesystem::path filepath, ShaderStageFlag type, const Device& device)
        : DeviceObject { device }, m_type { type } {
        auto stream     = std::ifstream { filepath.string(), std::ios::binary | std::ios::ate };
        const auto size = stream.tellg();

        m_source.resize(size);

        core::read(stream, core::viewAsBytes(m_source));

        compile();
        reflect();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(std::span<const core::Byte> data, ShaderStageFlag type, const Device& device)
        : Shader { core::viewAs<const SpirvID>(data), type, device } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(std::span<const SpirvID> data, ShaderStageFlag type, const Device& device)
        : DeviceObject { device }, m_type { type } {
        m_source.reserve(std::size(data));
        std::ranges::copy(data, std::back_inserter(m_source));

        compile();
        reflect();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::~Shader() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(Shader&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Shader::operator=(Shader&& other) noexcept -> Shader& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Shader::compile() -> void {
        const auto create_info = vk::ShaderModuleCreateInfo {}.setCode(m_source);

        try {
            m_vk_shader_module = vk::raii::ShaderModule { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Shader::reflect() noexcept -> void {
#ifdef STORMKIT_ENABLE_SPIRV_INTROSPECT
        auto ir = std::vector<SpirvID> {};
        ir.resize(std::size(m_source) / sizeof(SpirvID));
        std::memcpy(std::data(ir), std::data(m_source), std::size(m_source));

        auto compiler           = spirv_cross::CompilerGLSL { std::move(ir) };
        const auto add_bindings = [this,
                                   &compiler](core::span<const spirv_cross::Resource> resources,
                                              gpu::DescriptorType type) {
            for (const auto& resource : resources) {
                /*const auto set =
                    spvc_compiler_get_decoration(compiler, resources[i].id,
                   SpvDecorationDescriptorSet);*/
                const auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                // const auto name = spvc_compiler_get_name(compiler, resources[i].id);

                m_descriptor_set_layout.addBinding({ binding,
                                                     type,
                                                     gpu::ShaderStageFlag::Vertex |
                                                         gpu::ShaderStageFlag::Fragment |
                                                         gpu::ShaderStageFlag::Compute,
                                                     1 });
            }
        };

        auto resources = compiler.get_shader_resources();
        add_bindings(resources.uniform_buffers, DescriptorType::Uniform_Buffer);
        add_bindings(resources.storage_buffers, DescriptorType::Storage_Buffer);
        add_bindings(resources.sampled_images, DescriptorType::Sampled_Image);
        add_bindings(resources.storage_images, DescriptorType::Storage_Image);

#endif
        // m_descriptor_set_layout.bake();
    }
} // namespace stormkit::gpu
