// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC AbstractPipeline: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        AbstractPipeline(const Device &device, const PipelineCache *cache = nullptr) noexcept;

        virtual ~AbstractPipeline();

        AbstractPipeline(const AbstractPipeline &) = delete;
        auto operator=(const AbstractPipeline &) -> AbstractPipeline & = delete;

        AbstractPipeline(AbstractPipeline &&) noexcept;
        auto operator=(AbstractPipeline &&) noexcept -> AbstractPipeline &;

        [[nodiscard]] auto baked() const noexcept -> bool;
        virtual auto bake() -> void = 0;

        [[nodiscard]] operator VkPipeline() const noexcept;
        [[nodiscard]] VkPipeline vkHandle() const noexcept;
        [[nodiscard]] VkPipelineLayout vkLayout() const noexcept;

      protected:
        const PipelineCache *m_pipeline_cache = nullptr;

        bool m_is_baked = false;

        VkPipeline m_pipeline              = VK_NULL_HANDLE;
        VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

      private:
        AbstractPipeline() = delete;
    };
} // namespace stormkit::gpu

#include "AbstractPipeline.inl"
