// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#define STATE(TYPE, NAME, MEMBER)              \
    auto NAME() noexcept->TYPE & {             \
        STORMKIT_EXPECTS(m_is_baked == false); \
        return m_state.MEMBER;                 \
    }                                          \
    [[nodiscard]] auto NAME() const noexcept->const TYPE & { return m_state.MEMBER; }

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/AbstractPipeline.hpp>
#include <stormkit/gpu/pipeline/ComputePipelineState.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC ComputePipeline final: public AbstractPipeline {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        ComputePipeline(const Device &device, const PipelineCache *cache = nullptr) noexcept;
        ~ComputePipeline() override;

        ComputePipeline(const ComputePipeline &) = delete;
        auto operator=(const ComputePipeline &) -> ComputePipeline & = delete;

        ComputePipeline(ComputePipeline &&) noexcept;
        auto operator=(ComputePipeline &&) noexcept -> ComputePipeline &;

        STATE(ComputePipelineShaderState, shaderState, shader_state)
        STATE(ComputePipelineLayout, layout, layout)

        auto bake() -> void override;

        auto setState(const ComputePipelineState &state) noexcept -> void;
        [[nodiscard]] auto state() noexcept -> ComputePipelineState &;
        [[nodiscard]] auto state() const noexcept -> const ComputePipelineState &;

      private:
        ComputePipelineState m_state = {};
    };
} // namespace stormkit::gpu

#undef STATE

#include "ComputePipeline.inl"
