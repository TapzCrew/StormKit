// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/AbstractPipeline.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
AbstractPipeline::AbstractPipeline(const render::Device &device,
                                   PipelineCacheConstPtr cache) noexcept
    : m_device { &device }, m_pipeline_cache { cache } {};

/////////////////////////////////////
/////////////////////////////////////
AbstractPipeline::~AbstractPipeline() = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractPipeline::AbstractPipeline(AbstractPipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractPipeline &AbstractPipeline::operator=(AbstractPipeline &&) = default;
