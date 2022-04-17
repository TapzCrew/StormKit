// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/HashMap.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Shader.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Vertex.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC ResourceCache final: public core::NonCopyable {
      public:
        explicit ResourceCache(const Engine &engine);
        ~ResourceCache();

        ResourceCache(ResourceCache &&) noexcept;
        ResourceCache &operator=(ResourceCache &&) noexcept;

        render::DescriptorSetLayout &
            requestDescriptorSetLayout(render::DescriptorSetLayoutBindingConstSpan bindings);

        render::DescriptorSet &requestDescriptorSet(const render::DescriptorSetLayout &layout,
                                                    render::DescriptorConstSpan descriptors,
                                                    bool force_update = false);

        render::Shader &requestShader(render::ShaderStage stage, core::span<const core::Byte> data);
        render::Shader &requestShader(render::ShaderStage stage,
                                      core::span<const render::SpirvID> data);

        render::HardwareBuffer &requestGeometryBuffer(core::ByteConstSpan vertex_array,
                                                      core::ByteConstSpan index_array);

        render::GraphicsPipeline &
            requestGraphicsPipeline(const render::GraphicsPipelineState &state,
                                    const render::RenderPass &render_pass);
        render::RenderPass &requestRenderPass(render::RenderPassDescription description);

        ENGINE_CONST_GETTER
      private:
        engine::EngineConstRef m_engine;

        render::DescriptorPool m_descriptor_pool;

        core::HashMap<core::Hash64, render::DescriptorSetLayout> m_descriptor_set_layout_cache;
        core::HashMap<core::Hash64, render::DescriptorSet> m_descriptor_set_cache;
        core::HashMap<core::Hash64, render::Shader> m_shader_cache;
        core::HashMap<core::Hash64, render::HardwareBuffer> m_geometry_data_cache;
        core::HashMap<core::Hash64, render::RenderPass> m_render_pass_cache;

        render::PipelineCacheOwnedPtr m_pipeline_cache;
    };
} // namespace storm::engine
