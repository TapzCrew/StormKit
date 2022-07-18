// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/core/ShaderCache.hpp>

#include <stormkit/gpu/core/Device.hpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    ShaderCache::ShaderCache(const gpu::Device &device) : m_device { &device } {}

    /////////////////////////////////////
    /////////////////////////////////////
    ShaderCache::~ShaderCache() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    ShaderCache::ShaderCache(ShaderCache &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto ShaderCache::operator=(ShaderCache &&other) noexcept -> ShaderCache & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto ShaderCache::load(std::string name,
                           std::filesystem::path filepath,
                           gpu::ShaderStageFlag type) -> const gpu::Shader & {
        auto shader = m_device->createShader(std::move(filepath), type);

        return insert(std::move(name), std::move(shader));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ShaderCache::load(std::string name, core::ByteConstSpan data, gpu::ShaderStageFlag type)
        -> const gpu::Shader & {
        auto shader = m_device->createShader(data, type);

        return insert(std::move(name), std::move(shader));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ShaderCache::load(std::string name,
                           std::span<const gpu::SpirvID> data,
                           gpu::ShaderStageFlag type) -> const gpu::Shader & {
        auto shader = m_device->createShader(data, type);

        return insert(std::move(name), std::move(shader));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ShaderCache::insert(std::string name, gpu::Shader &&shader) -> const gpu::Shader & {
        const auto [it, _] = m_shaders.emplace(std::move(name), std::move(shader));

        m_device->setObjectName(it->second, it->first);

        return it->second;
    }
} // namespace stormkit::engine
