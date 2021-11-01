// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    struct TextureDescriptor {
        using PhysicalResource = render::Texture;

        render::TextureType type;
        render::PixelFormat format;

        core::Extentu extent;

        core::UInt8 levels              = 1;
        render::SampleCountFlag samples = render::SampleCountFlag::C1_BIT;

        render::TextureUsage usage = static_cast<render::TextureUsage>(0);
    };

    struct BufferDescriptor {
        using PhysicalResource = render::HardwareBuffer;

        core::ArraySize size;

        render::HardwareBufferUsage usage;
        render::MemoryProperty memory_properties;
    };

    using Descriptor = std::variant<TextureDescriptor, BufferDescriptor>;

    class STORMKIT_PUBLIC FrameGraphResource: public core::NonCopyable {
      public:
        enum class Type { Texture, Buffer };

        FrameGraphResource(std::string name,
                           FrameGraphResourceID id,
                           bool transient,
                           Descriptor descriptor) noexcept;
        ~FrameGraphResource();

        FrameGraphResource(FrameGraphResource &&) noexcept;
        FrameGraphResource &operator=(FrameGraphResource &&) noexcept;

        [[nodiscard]] FrameGraphResourceID id() const noexcept;
        [[nodiscard]] std::string_view name() const noexcept;
        [[nodiscard]] bool isTransient() const noexcept;
        [[nodiscard]] bool isCullImune() const noexcept;

        [[nodiscard]] Type type() const noexcept;
        [[nodiscard]] const Descriptor &descriptor() const noexcept;

        [[nodiscard]] details::FrameGraphNodeID createdBy() const noexcept;
        [[nodiscard]] details::FrameGraphNodeIDConstSpan readIn() const noexcept;
        [[nodiscard]] details::FrameGraphNodeIDConstSpan writedIn() const noexcept;

        ALLOCATE_HELPERS(FrameGraphResource)
      private:
        FrameGraphResourceID m_id;
        std::string m_name;
        bool m_is_transient;
        bool m_is_cull_immune = false;

        details::FrameGraphNodeID m_created_by;
        details::FrameGraphNodeIDArray m_read_in;
        details::FrameGraphNodeIDArray m_writed_in;

        core::UInt32 m_ref_count = 0u;

        Type m_type;
        Descriptor m_descriptor;

        friend class storm::engine::FrameGraph;
        friend class storm::engine::FrameGraphBuilder;
    };
} // namespace storm::engine

#include "FrameGraphResource.inl"
