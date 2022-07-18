// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>
#include <span>
#include <string_view>

#include <stormkit/core/HashMap.hpp>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Strings.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/gpu/resource/Shader.hpp>

namespace stormkit::engine {
    class ShaderCache {
      public:
        ShaderCache(const gpu::Device &device);
        ~ShaderCache();

        ShaderCache(const ShaderCache &) = delete;
        auto operator=(const ShaderCache &) -> ShaderCache & = delete;

        ShaderCache(ShaderCache &&) noexcept;
        auto operator=(ShaderCache &&) noexcept -> ShaderCache &;

        [[nodiscard]] auto has(std::string_view name) const noexcept -> bool;
        [[nodiscard]] auto get(std::string_view name) const noexcept -> const gpu::Shader &;

        auto load(std::string name, std::filesystem::path filepath, gpu::ShaderStageFlag type)
            -> const gpu::Shader &;
        auto load(std::string name, core::ByteConstSpan data, gpu::ShaderStageFlag type)
            -> const gpu::Shader &;
        auto load(std::string name, std::span<const gpu::SpirvID> data, gpu::ShaderStageFlag type)
            -> const gpu::Shader &;

      private:
        auto insert(std::string name, gpu::Shader &&shader) -> const gpu::Shader &;

        const gpu::Device *m_device = nullptr;

        core::StringHashMap<gpu::Shader> m_shaders;
    };
} // namespace stormkit::engine

#include "ShaderCache.inl"
