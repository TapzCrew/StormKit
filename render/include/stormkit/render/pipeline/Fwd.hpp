// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::render {
    struct RenderPassDescription;
    struct VertexBindingDescription;
    struct VertexInputAttributeDescription;

    class AbstractPipeline;
    DECLARE_PTR_AND_REF(AbstractPipeline);

    class GraphicsPipeline;
    DECLARE_PTR_AND_REF(GraphicsPipeline);

    class ComputePipeline;
    DECLARE_PTR_AND_REF(ComputePipeline);

    class RenderPass;
    DECLARE_PTR_AND_REF(RenderPass);

    class Framebuffer;
    DECLARE_PTR_AND_REF(Framebuffer);

    class DescriptorSetLayout;
    DECLARE_PTR_AND_REF(DescriptorSetLayout);

    class DescriptorPool;
    DECLARE_PTR_AND_REF(DescriptorPool);

    class DescriptorSet;
    DECLARE_PTR_AND_REF(DescriptorSet);

    class PipelineCache;
    DECLARE_PTR_AND_REF(PipelineCache);

    struct GraphicsPipelineState;
    DECLARE_PTR_AND_REF(GraphicsPipelineState);

    struct ComputePipelineState;
    DECLARE_PTR_AND_REF(ComputePipelineState);

    struct DescriptorSetLayoutBinding;
    DECLARE_PTR_AND_REF(DescriptorSetLayoutBinding);
} // namespace stormkit::render
