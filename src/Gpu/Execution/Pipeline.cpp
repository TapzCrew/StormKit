// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Execution.Pipeline;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::Pipeline(const gpu::Device& device, const PipelineCache *cache) noexcept
        : DeviceObject { device }, m_pipeline_cache { cache } {};

    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::~Pipeline() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::Pipeline(Pipeline&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline& = default;
} // namespace stormkit::gpu
