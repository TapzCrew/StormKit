// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Buffer: public DeviceObject {
      public:
        struct CreateInfo {
            BufferUsageFlag usages;
            core::USize size;
            MemoryPropertyFlag property =
                MemoryPropertyFlag::Host_Visible | MemoryPropertyFlag::Host_Coherent;
        };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Buffer;

        Buffer(const Device &device, const CreateInfo &info, bool persistently_mapping = false);
        ~Buffer();

        Buffer(const Buffer &) = delete;
        auto operator          =(const Buffer &) -> Buffer &;

        Buffer(Buffer &&) noexcept;
        auto operator=(Buffer &&) noexcept -> Buffer &;

        [[nodiscard]] auto usages() const noexcept -> BufferUsageFlag;
        [[nodiscard]] auto size() const noexcept -> core::USize;

        [[nodiscard]] auto map(core::Int64 offset) noexcept -> core::Byte *;
        [[nodiscard]] auto map(core::Int64 offset, core::USize size) noexcept -> core::ByteSpan;

        template<typename T>
        [[nodiscard]] auto mapAs(core::Int64 offset) -> T *;

        [[nodiscard]] auto data() noexcept -> core::Byte *;
        [[nodiscard]] auto data() const noexcept -> const core::Byte *;
        [[nodiscard]] auto data(core::USize size) noexcept -> core::ByteSpan;
        [[nodiscard]] auto data(core::USize size) const noexcept -> core::ByteConstSpan;

        template<typename T>
        [[nodiscard]] auto dataAs() noexcept -> T *;
        template<typename T>
        [[nodiscard]] auto dataAs() const noexcept -> const T *;

        auto flush(core::Int64 offset, core::USize size) -> void;
        auto unmap() -> void;

        [[nodiscard]] auto isPersistentlyMapped() const noexcept -> bool;

        auto upload(core::ByteConstSpan data, core::Int64 offset = 0) -> void;
        template<typename T>
        auto upload(const T &data, core::Int64 offset = 0) -> void;
        template<typename T>
        auto upload(std::span<const T> data, core::Int64 offset = 0) -> void;

        [[nodiscard]] operator VkBuffer() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkBuffer;

      private:
        static auto findMemoryType(core::UInt32 type_filter,
                                   VkMemoryPropertyFlags properties,
                                   const VkPhysicalDeviceMemoryProperties &mem_properties,
                                   const VkMemoryRequirements &mem_requirements) -> core::UInt32;

        BufferUsageFlag m_usages = {};
        core::USize m_size       = 0;

        bool m_is_persistently_mapped = false;
        core::Byte *m_mapped_pointer  = nullptr;

        VkBuffer m_buffer             = VK_NULL_HANDLE;
        VmaAllocation m_buffer_memory = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "Buffer.inl"