// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC AbstractPipeline: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        explicit AbstractPipeline(const Device &device,
                                  PipelineCacheConstPtr cache = nullptr) noexcept;

        virtual ~AbstractPipeline();

        AbstractPipeline(AbstractPipeline &&);
        AbstractPipeline &operator=(AbstractPipeline &&);

        inline bool isBuilded() const noexcept;
        virtual void build() = 0;

        inline vk::Pipeline vkPipeline() const noexcept;
        inline vk::PipelineLayout vkPipelineLayout() const noexcept;
        inline operator vk::Pipeline() const noexcept;
        inline vk::Pipeline vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      protected:
        DeviceConstPtr m_device;
        PipelineCacheConstPtr m_pipeline_cache;

        bool m_is_builded = false;

        RAIIVkPipeline m_vk_pipeline;
        RAIIVkPipelineLayout m_vk_pipeline_layout;
    };
} // namespace storm::render

#undef STATE

#include "AbstractPipeline.inl"
