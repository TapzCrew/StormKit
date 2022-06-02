// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <fstream>
#include <streambuf>

#ifdef STORMKIT_ENABLE_SPIRV_INTROSPECT
    /////////// - spirv-cross - ///////////
    #include <spirv_glsl.hpp>
#endif

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>

#include <stormkit/gpu/resource/Shader.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(std::filesystem::path filepath, ShaderStageFlag type, const Device &device)
        : DeviceObject { device }, m_type { type } /* , m_descriptor_set_layout {
                                            m_device->createDescriptorSetLayout()
                                            } */
    {
        auto stream =
            std::basic_ifstream<std::byte> { filepath.string(), std::ios::binary | std::ios::ate };
        auto size = stream.tellg();

        m_source.resize(size);
        stream.read(std::data(m_source), size);

        compile();
        reflect();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(core::ByteConstSpan data, ShaderStageFlag type, const Device &device)
        : DeviceObject { device }, m_type {
              type
          } /*, m_descriptor_set_layout {
m_device->createDescriptorSetLayout()
}*/ {
        m_source.reserve(std::size(data));

        std::ranges::copy(data, std::back_inserter(m_source));
        compile();
        reflect();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(std::span<const SpirvID> data, ShaderStageFlag type, const Device &device)
        : Shader { { reinterpret_cast<const core::Byte *>(std::data(data)),
                     std::size(data) * sizeof(SpirvID) },
                   type,
                   device } {}

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::~Shader() {
        if (m_shader_module != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyShaderModule(device(), m_shader_module, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Shader::Shader(Shader &&other) noexcept
        : DeviceObject { std::move(other) }, m_type { std::exchange(other.m_type,
                                                                    ShaderStageFlag::None) },
          m_source { std::move(other.m_source) }, m_shader_module {
              std::exchange(other.m_shader_module, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Shader::operator=(Shader &&other) noexcept -> Shader & {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_type                = std::exchange(other.m_type, ShaderStageFlag::None);
        m_source              = std::move(other.m_source);
        m_shader_module       = std::exchange(other.m_shader_module, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Shader::compile() noexcept -> void {
        const auto &vk = device().table();

        const auto create_info =
            VkShaderModuleCreateInfo { .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                       .codeSize = core::as<core::UInt32>(std::size(m_source)),
                                       .pCode =
                                           reinterpret_cast<const SpirvID *>(std::data(m_source)) };

        CHECK_VK_ERROR(vk.vkCreateShaderModule(device(), &create_info, nullptr, &m_shader_module));
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
            for (const auto &resource : resources) {
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
