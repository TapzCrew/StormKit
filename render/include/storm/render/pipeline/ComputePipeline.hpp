// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/AbstractPipeline.hpp>
#include <storm/render/pipeline/ComputePipelineState.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#define STATE(TYPE, NAME, MEMBER)                \
    inline TYPE &NAME() noexcept {               \
        STORMKIT_EXPECTS(m_is_builded == false); \
        return m_state.MEMBER;                   \
    }                                            \
    inline const TYPE &NAME() const noexcept { return m_state.MEMBER; }

namespace storm::render {
    class STORMKIT_PUBLIC ComputePipeline final: public AbstractPipeline {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        explicit ComputePipeline(const Device &device,
                                 PipelineCacheConstPtr cache = nullptr) noexcept;

        ~ComputePipeline() override;

        ComputePipeline(ComputePipeline &&);
        ComputePipeline &operator=(ComputePipeline &&);

        STATE(ComputePipelineShaderState, shaderState, shader_state)
        STATE(ComputePipelineLayout, layout, layout)

        void build() override;

        inline void setState(ComputePipelineState state) noexcept;
        inline const ComputePipelineState &state() const noexcept;
        inline ComputePipelineState &state() noexcept;

      private:
        ComputePipelineState m_state;

        RenderPassConstPtr m_render_pass;
    };
} // namespace storm::render

#undef STATE

#include "ComputePipeline.inl"
