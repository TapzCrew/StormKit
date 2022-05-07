// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::engine - ///////////
#include "../../Log.hpp"
#include <storm/engine/render/utils/ResourceCache.hpp>

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

//#include <storm/engine/render/Material.hpp>

using namespace storm;
using namespace storm::engine;

template<typename... Args>
core::Hash64 hashParams(Args &&...args) {
    auto hash = core::Hash64 { 0u };

    (core::hashCombine(hash, std::forward<Args>(args)), ...);

    return hash;
}

static constexpr auto DESCRIPTOR_COUNT = 1000;

template<typename T, auto Maker, typename... Args>
T &requestResource(core::HashMap<core::Hash64, T> &cache,
                   core::Hash64 hash,
                   Args &&...args) noexcept {
    auto is_in_cache =
        core::ranges::any_of(cache, [&hash](const auto &pair) { return pair.first == hash; });

    if (is_in_cache) return cache.at(hash);

    const auto type = typeid(T).name();
    const auto id   = std::size(cache);

    dlog("Build cache for {} ({})", type, id);

    auto resource = Maker(std::forward<Args>(args)...);

    return cache.emplace(hash, std::move(resource)).first->second;
}

/////////////////////////////////////
/////////////////////////////////////
ResourceCache::ResourceCache(const Engine &engine)
    : m_engine { engine }, m_descriptor_pool {
          engine.device(),
          {
              { storm::render::DescriptorType::Sampler, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Combined_Texture_Sampler, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Sampled_Image, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Storage_Image, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Uniform_Texel_Buffer, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Storage_Texel_Buffer, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Uniform_Buffer, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Storage_Buffer, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Uniform_Buffer_Dynamic, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Storage_Buffer_Dynamic, DESCRIPTOR_COUNT },
              { storm::render::DescriptorType::Input_Attachment, DESCRIPTOR_COUNT },
          },
          DESCRIPTOR_COUNT
      } {
    m_pipeline_cache = engine.device().createPipelineCachePtr();
}

/////////////////////////////////////
/////////////////////////////////////
ResourceCache::~ResourceCache() = default;

/////////////////////////////////////
/////////////////////////////////////
ResourceCache::ResourceCache(ResourceCache &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
ResourceCache &ResourceCache::operator=(ResourceCache &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
render::DescriptorSetLayout &ResourceCache::requestDescriptorSetLayout(
    render::DescriptorSetLayoutBindingConstSpan bindings) {
    constexpr auto maker = [](const Engine &engine,
                              render::DescriptorSetLayoutBindingConstSpan bindings) {
        auto layout = engine.device().createDescriptorSetLayout();

        for (const auto &binding : bindings) layout.addBinding(binding);

        layout.bake();

        return layout;
    };

    const auto hash = hashParams(bindings);

#ifdef STORMKIT_COMPILER_CLANG
    return requestResource<render::DescriptorSetLayout, +maker>(m_descriptor_set_layout_cache,
                                                                hash,
                                                                engine(),
                                                                bindings);
#else
    return requestResource<render::DescriptorSetLayout, maker>(m_descriptor_set_layout_cache,
                                                               hash,
                                                               engine(),
                                                               bindings);

#endif
}

/////////////////////////////////////
/////////////////////////////////////
render::DescriptorSet &
    ResourceCache::requestDescriptorSet(const render::DescriptorSetLayout &layout,
                                        render::DescriptorConstSpan descriptors,
                                        bool force_update) {
    constexpr auto maker = [](render::DescriptorPool &pool,
                              const render::DescriptorSetLayout &layout,
                              render::DescriptorConstSpan descriptors) {
        auto descriptor_set = pool.allocateDescriptorSet(layout);
        descriptor_set.update(descriptors);

        return descriptor_set;
    };

    const auto hash = hashParams(layout, descriptors);

#ifdef STORMKIT_COMPILER_CLANG
    auto &resource = requestResource<render::DescriptorSet, +maker>(m_descriptor_set_cache,
                                                                    hash,
                                                                    m_descriptor_pool,
                                                                    layout,
                                                                    descriptors);
#else
    auto &resource = requestResource<render::DescriptorSet, maker>(m_descriptor_set_cache,
                                                                   hash,
                                                                   m_descriptor_pool,
                                                                   layout,
                                                                   descriptors);
#endif
    resource.update(descriptors);

    return resource;
}

/////////////////////////////////////
/////////////////////////////////////
render::Shader &ResourceCache::requestShader(render::ShaderStage stage, core::ByteConstSpan data) {
    constexpr auto maker =
        [](const Engine &engine, render::ShaderStage stage, core::ByteConstSpan data) {
            return engine.device().createShader(data, stage);
        };

    const auto hash = hashParams(stage, data);

#ifdef STORMKIT_COMPILER_CLANG
    return requestResource<render::Shader, +maker>(m_shader_cache, hash, engine(), stage, data);
#else
    return requestResource<render::Shader, maker>(m_shader_cache, hash, engine(), stage, data);
#endif
}

/////////////////////////////////////
/////////////////////////////////////
render::Shader &ResourceCache::requestShader(render::ShaderStage stage,
                                             core::span<const render::SpirvID> data) {
    return requestShader(stage,
                         { reinterpret_cast<const core::Byte *>(std::data(data)),
                           std::size(data) * sizeof(render::SpirvID) });
}

/////////////////////////////////////
/////////////////////////////////////
render::HardwareBuffer &ResourceCache::requestGeometryBuffer(core::ByteConstSpan vertex_array,
                                                             core::ByteConstSpan index_array) {
    constexpr auto maker = [](const Engine &engine,
                              core::ByteConstSpan vertex_array,
                              core::ByteConstSpan index_array) {
        return engine.device().createVertexBuffer(std::size(vertex_array) + std::size(index_array),
                                                  render::MemoryProperty::Device_Local,
                                                  true);
    };
    const auto hash = hashParams(vertex_array, index_array);

#ifdef STORMKIT_COMPILER_CLANG
    return requestResource<render::HardwareBuffer, +maker>(m_geometry_data_cache,
                                                           hash,
                                                           engine(),
                                                           vertex_array,
                                                           index_array);
#else
    return requestResource<render::HardwareBuffer, maker>(m_geometry_data_cache,
                                                          hash,
                                                          engine(),
                                                          vertex_array,
                                                          index_array);
#endif
}

/////////////////////////////////////
/////////////////////////////////////
render::GraphicsPipeline &
    ResourceCache::requestGraphicsPipeline(const render::GraphicsPipelineState &state,
                                           const render::RenderPass &render_pass) {
    return m_pipeline_cache->getPipeline(state, render_pass);
}

/////////////////////////////////////
/////////////////////////////////////
render::RenderPass &ResourceCache::requestRenderPass(render::RenderPassDescription description) {
    constexpr auto maker = [](const Engine &engine, render::RenderPassDescription &&description) {
        return engine.device().createRenderPass(std::move(description));
    };
    const auto hash = hashParams(description);

#ifdef STORMKIT_COMPILER_CLANG
    return requestResource<render::RenderPass, +maker>(m_render_pass_cache,
                                                       hash,
                                                       engine(),
                                                       std::move(description));
#else
    return requestResource<render::RenderPass, maker>(m_render_pass_cache,
                                                      hash,
                                                      engine(),
                                                      std::move(description));
#endif
}
