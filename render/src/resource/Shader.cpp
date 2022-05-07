// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <codecvt>
#include <fstream>
#include <streambuf>

#ifdef STORMKIT_ENABLE_SPIRV_INTROSPECT
/////////// - spirv-cross - ///////////
#include <spirv_glsl.hpp>
#endif

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;
using namespace storm::log;

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(std::filesystem::path filepath, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type }, m_descriptor_set_layout {
          m_device->createDescriptorSetLayout()
      } {
    auto stream     = std::ifstream { filepath.string(), std::ios::binary };
    const auto data = std::vector<char> { (std::istreambuf_iterator<char> { stream }),
                                          std::istreambuf_iterator<char> {} };
    std::transform(core::ranges::begin(data),
                   core::ranges::end(data),
                   std::back_inserter(m_source),
                   [](const auto b) { return static_cast<std::byte>(b); });
    compile();
    reflect();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const core::Byte> data, ShaderStage type, const Device &device)
    : m_device { &device }, m_type { type }, m_descriptor_set_layout {
          m_device->createDescriptorSetLayout()
      } {
    m_source.resize(std::size(data));
    std::copy(std::cbegin(data), std::cend(data), core::ranges::begin(m_source));
    compile();
    reflect();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const SpirvID> data, ShaderStage type, const Device &device)
    : Shader { { reinterpret_cast<const core::Byte *>(std::data(data)),
                 std::size(data) * sizeof(SpirvID) },
               type,
               device } {
}

/////////////////////////////////////
/////////////////////////////////////
Shader::~Shader() = default;

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Shader &Shader::operator=(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Shader::compile() noexcept {
    const auto create_info =
        vk::ShaderModuleCreateInfo {}
            .setCodeSize(std::size(m_source))
            .setPCode(reinterpret_cast<const core::UInt32 *>(std::data(m_source)));

    m_vk_shader_module = m_device->createVkShaderModule(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
void Shader::reflect() noexcept {
#ifdef STORMKIT_ENABLE_SPIRV_INTROSPECT
    auto ir = std::vector<core::UInt32> {};
    ir.resize(std::size(m_source) / sizeof(core::UInt32));
    std::memcpy(std::data(ir), std::data(m_source), std::size(m_source));

    auto compiler           = spirv_cross::CompilerGLSL { std::move(ir) };
    const auto add_bindings = [this, &compiler](core::span<const spirv_cross::Resource> resources,
                                                render::DescriptorType type) {
        for (const auto &resource : resources) {
            /*const auto set =
                spvc_compiler_get_decoration(compiler, resources[i].id,
               SpvDecorationDescriptorSet);*/
            const auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            // const auto name = spvc_compiler_get_name(compiler, resources[i].id);

            m_descriptor_set_layout.addBinding({ binding,
                                                 type,
                                                 render::ShaderStage::Vertex |
                                                     render::ShaderStage::Fragment |
                                                     render::ShaderStage::Compute,
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
