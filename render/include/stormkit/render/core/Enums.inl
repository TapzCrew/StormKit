// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    inline auto to_string(PhysicalDeviceType type) -> std::string {
        switch (type) {
            case PhysicalDeviceType::CPU: return "CPU";
            case PhysicalDeviceType::Virtual_GPU: return "Virtual_GPU";
            case PhysicalDeviceType::Discrete_GPU: return "Discrete_GPU";
            case PhysicalDeviceType::Integrated_GPU: return "core::Integrated_GPU";
        }

        return "Unknown";
    }

    constexpr auto isDepthOnlyFormat(PixelFormat format) noexcept -> bool {
        return format == PixelFormat::Depth16 || format == PixelFormat::Depth24 ||
               format == PixelFormat::Depth32F;
    }

    constexpr auto isDepthStencilFormat(PixelFormat format) noexcept -> bool {
        return format == PixelFormat::Depth16_Stencil8 || format == PixelFormat::Depth24_Stencil8 ||
               format == PixelFormat::Depth32F_Stencil8;
    }

    constexpr auto isDepthFormat(PixelFormat format) noexcept -> bool {
        return isDepthOnlyFormat(format) || isDepthStencilFormat(format);
    }

    constexpr auto getChannelCountFor(PixelFormat format) noexcept -> core::UInt8 {
        switch (format) {
            case PixelFormat::R8_SNorm:
            case PixelFormat::R8_UNorm:
            case PixelFormat::R16_SNorm:
            case PixelFormat::R16_UNorm:
            case PixelFormat::R8I:
            case PixelFormat::R8U:
            case PixelFormat::R16I:
            case PixelFormat::R16U:
            case PixelFormat::R32I:
            case PixelFormat::R32U:
            case PixelFormat::R16F:
            case PixelFormat::R32F:
            case PixelFormat::Depth16:
            case PixelFormat::Depth24:
            case PixelFormat::Depth32F: return 1;

            case PixelFormat::RG8_SNorm:
            case PixelFormat::RG8_UNorm:
            case PixelFormat::RG16_SNorm:
            case PixelFormat::RG16_UNorm:
            case PixelFormat::RG8I:
            case PixelFormat::RG8U:
            case PixelFormat::RG16I:
            case PixelFormat::RG16U:
            case PixelFormat::RG32I:
            case PixelFormat::RG32U:
            case PixelFormat::RG16F:
            case PixelFormat::RG32F:
            case PixelFormat::Depth16_Stencil8:
            case PixelFormat::Depth24_Stencil8:
            case PixelFormat::Depth32F_Stencil8: return 2;

            case PixelFormat::RGB8_SNorm:
            case PixelFormat::RGB8_UNorm:
            case PixelFormat::RGB16_SNorm:
            case PixelFormat::RGB16_UNorm:
            case PixelFormat::BGR8_UNorm:
            case PixelFormat::RGB8I:
            case PixelFormat::RGB8U:
            case PixelFormat::RGB16I:
            case PixelFormat::RGB16U:
            case PixelFormat::RGB32I:
            case PixelFormat::RGB32U:
            case PixelFormat::RGB16F:
            case PixelFormat::RGB32F:
            case PixelFormat::sRGB8:
            case PixelFormat::sBGR8:
            case PixelFormat::R5_G6_B5_UNorm_Pack16:
            case PixelFormat::BG11_R10F_Pack32: return 3;

            case PixelFormat::RGBA8_SNorm:
            case PixelFormat::RGBA8_UNorm:
            case PixelFormat::RGBA16_SNorm:
            case PixelFormat::RGBA16_UNorm:
            case PixelFormat::BGRA8_UNorm:
            case PixelFormat::RGBA8I:
            case PixelFormat::RGBA8U:
            case PixelFormat::RGBA16I:
            case PixelFormat::RGBA16U:
            case PixelFormat::RGBA32I:
            case PixelFormat::RGBA32U:
            case PixelFormat::RGBA16F:
            case PixelFormat::RGBA32F:
            case PixelFormat::sRGBA8:
            case PixelFormat::sBGRA8: return 4;

            default: return 0u;
        }

        return 0u;
    }

    constexpr auto getArraySizeByChannelFor(PixelFormat format) noexcept -> core::UInt8 {
        switch (format) {
            case PixelFormat::R8_SNorm:
            case PixelFormat::R8_UNorm:
            case PixelFormat::RG8_SNorm:
            case PixelFormat::RG8_UNorm:
            case PixelFormat::R8I:
            case PixelFormat::R8U:
            case PixelFormat::RG8I:
            case PixelFormat::RG8U:
            case PixelFormat::RGB8_SNorm:
            case PixelFormat::RGB8_UNorm:
            case PixelFormat::BGR8_UNorm:
            case PixelFormat::RGB8I:
            case PixelFormat::RGB8U:
            case PixelFormat::RGBA8_SNorm:
            case PixelFormat::RGBA8_UNorm:
            case PixelFormat::RGBA16_SNorm:
            case PixelFormat::BGRA8_UNorm:
            case PixelFormat::sRGB8:
            case PixelFormat::sBGR8:
            case PixelFormat::sRGBA8:
            case PixelFormat::sBGRA8: return 1u;

            case PixelFormat::R16_SNorm:
            case PixelFormat::R16_UNorm:
            case PixelFormat::R16I:
            case PixelFormat::R16U:
            case PixelFormat::RG16_SNorm:
            case PixelFormat::RG16_UNorm:
            case PixelFormat::RG16I:
            case PixelFormat::RG16U:
            case PixelFormat::RG16F:
            case PixelFormat::RGB16I:
            case PixelFormat::RGB16U:
            case PixelFormat::RGB16F:
            case PixelFormat::RGBA16I:
            case PixelFormat::RGBA16U:
            case PixelFormat::RGBA16F:
            case PixelFormat::R16F: return 2u;

            case PixelFormat::R32I:
            case PixelFormat::R32U:
            case PixelFormat::R32F:
            case PixelFormat::RG32I:
            case PixelFormat::RG32U:
            case PixelFormat::RG32F:
            case PixelFormat::RGB16_SNorm:
            case PixelFormat::RGB32I:
            case PixelFormat::RGB32U:
            case PixelFormat::RGB32F:
            case PixelFormat::RGBA8I:
            case PixelFormat::RGBA8U:
            case PixelFormat::RGBA32I:
            case PixelFormat::RGBA32U:
            case PixelFormat::RGBA32F: return 4u;

            default: return 0u;
        }

        return 0u;
    }
} // namespace stormkit::render
