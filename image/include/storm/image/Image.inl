// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Image.hpp"

namespace storm::image {
    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::ArraySize index,
                                core::UInt32 layer,
                                core::UInt32 face,
                                core::UInt32 level) noexcept {
        STORMKIT_EXPECTS(m_mip_levels > level);
        STORMKIT_EXPECTS(m_faces > face);
        STORMKIT_EXPECTS(m_layers > layer);

        auto _data = data(layer, face, level);

        STORMKIT_EXPECTS(index < m_extent.width * m_extent.height * m_extent.depth);

        const auto block_size = m_channel_count * m_bytes_per_channel;

        return { std::data(_data) + index * block_size, block_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::ArraySize index,
                                     core::UInt32 layer,
                                     core::UInt32 face,
                                     core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_mip_levels > level);
        STORMKIT_EXPECTS(m_faces > face);
        STORMKIT_EXPECTS(m_layers > layer);

        auto _data = data(layer, face, level);

        const auto mip_extent = extent(level);
        STORMKIT_EXPECTS(index < mip_extent.width * mip_extent.height * mip_extent.depth);

        const auto block_size = m_channel_count * m_bytes_per_channel;

        return { std::data(_data) + index * block_size, block_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::Position3u position,
                                core::UInt32 layer,
                                core::UInt32 face,
                                core::UInt32 level) noexcept {
        const auto mip_extent = extent(level);

        const auto id = position->x + (position->y * mip_extent.width) +
                        (mip_extent.width * mip_extent.height * position->z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::Position3u position,
                                     core::UInt32 layer,
                                     core::UInt32 face,
                                     core::UInt32 level) const noexcept {
        const auto mip_extent = extent(level);

        const auto id = position->x + (position->y * mip_extent.width) +
                        (mip_extent.width * mip_extent.height * position->z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::Extentu Image::extent(core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_mip_levels > level);

        return { std::max(1u, m_extent.width >> level),
                 std::max(1u, m_extent.height >> level),
                 std::max(1u, m_extent.depth >> level) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Image::channelCount() const noexcept { return m_channel_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Image::bytesPerChannel() const noexcept { return m_bytes_per_channel; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Image::layers() const noexcept { return m_layers; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Image::faces() const noexcept { return m_faces; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Image::mipLevels() const noexcept { return m_mip_levels; }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Format Image::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size() const noexcept { return std::size(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize
        Image::size(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_mip_levels > level);
        STORMKIT_EXPECTS(m_faces > face);
        STORMKIT_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        return mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count *
               m_bytes_per_channel;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size(core::UInt32 layer, core::UInt32 face) const noexcept {
        auto _size = core::ArraySize { 0u };
        for (auto i = 0u; i < m_mip_levels; ++i) _size += size(layer, face, i);

        return _size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size(core::UInt32 layer) const noexcept {
        auto _size = core::ArraySize { 0u };
        for (auto i = 0u; i < m_faces; ++i) _size += size(layer, i);

        return _size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data() noexcept { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        const auto mip_size = size(layer, face, level);

        auto offset = 0u;

        for (auto i = 0u; i < layer; ++i) offset += size(i);

        for (auto j = 0u; j < face; ++j) offset += size(layer, j);

        for (auto k = 0u; k < level; ++k) offset += size(layer, face, k);

        return { std::data(m_data) + offset, mip_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::data() const noexcept { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan
        Image::data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        const auto mip_size = size(layer, face, level);

        auto offset = 0u;

        for (auto i = 0u; i < layer; ++i) offset += size(i);

        for (auto j = 0u; j < face; ++j) offset += size(layer, j);

        for (auto k = 0u; k < level; ++k) offset += size(layer, face, k);

        return { std::data(m_data) + offset, mip_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::begin() noexcept { return std::begin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan::iterator
        Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORMKIT_EXPECTS(m_mip_levels > level);
        STORMKIT_EXPECTS(m_faces > face);
        STORMKIT_EXPECTS(m_layers > layer);

        return std::begin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::begin() const noexcept { return std::begin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan::iterator
        Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return std::begin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cbegin() const noexcept { return std::cbegin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan::iterator
        Image::cbegin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return std::cbegin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::end() noexcept { return std::end(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan::iterator
        Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        return std::end(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::end() const noexcept { return std::end(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan::iterator
        Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return std::end(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cend() const noexcept { return std::cend(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan::iterator
        Image::cend(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return std::cend(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr core::UInt8 getChannelCountFor(Image::Format format) noexcept {
        switch (format) {
            case Image::Format::R8_SNorm:
            case Image::Format::R8_UNorm:
            case Image::Format::R16_SNorm:
            case Image::Format::R16_UNorm:
            case Image::Format::R8I:
            case Image::Format::R8U:
            case Image::Format::R16I:
            case Image::Format::R16U:
            case Image::Format::R32I:
            case Image::Format::R32U:
            case Image::Format::R16F:
            case Image::Format::R32F: return 1;

            case Image::Format::RG8_SNorm:
            case Image::Format::RG8_UNorm:
            case Image::Format::RG16_SNorm:
            case Image::Format::RG16_UNorm:
            case Image::Format::RG8I:
            case Image::Format::RG8U:
            case Image::Format::RG16I:
            case Image::Format::RG16U:
            case Image::Format::RG32I:
            case Image::Format::RG32U:
            case Image::Format::RG16F:
            case Image::Format::RG32F: return 2;

            case Image::Format::RGB8_SNorm:
            case Image::Format::RGB8_UNorm:
            case Image::Format::RGB16_SNorm:
            case Image::Format::RGB16_UNorm:
            case Image::Format::BGR8_UNorm:
            case Image::Format::RGB8I:
            case Image::Format::RGB8U:
            case Image::Format::RGB16I:
            case Image::Format::RGB16U:
            case Image::Format::RGB32I:
            case Image::Format::RGB32U:
            case Image::Format::RGB16F:
            case Image::Format::RGB32F:
            case Image::Format::sRGB8:
            case Image::Format::sBGR8: return 3;

            case Image::Format::RGBA8_SNorm:
            case Image::Format::RGBA8_UNorm:
            case Image::Format::RGBA16_SNorm:
            case Image::Format::RGBA16_UNorm:
            case Image::Format::BGRA8_UNorm:
            case Image::Format::RGBA8I:
            case Image::Format::RGBA8U:
            case Image::Format::RGBA16I:
            case Image::Format::RGBA16U:
            case Image::Format::RGBA32I:
            case Image::Format::RGBA32U:
            case Image::Format::RGBA16F:
            case Image::Format::RGBA32F:
            case Image::Format::sRGBA8:
            case Image::Format::sBGRA8: return 4;

            default: return 0u;
        }

        return 0u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr core::UInt8 getArraySizeByChannelFor(Image::Format format) noexcept {
        switch (format) {
            case Image::Format::R8_SNorm:
            case Image::Format::R8_UNorm:
            case Image::Format::RG8_SNorm:
            case Image::Format::RG8_UNorm:
            case Image::Format::R8I:
            case Image::Format::R8U:
            case Image::Format::RG8I:
            case Image::Format::RG8U:
            case Image::Format::RGB8_SNorm:
            case Image::Format::RGB8_UNorm:
            case Image::Format::BGR8_UNorm:
            case Image::Format::RGB8I:
            case Image::Format::RGB8U:
            case Image::Format::RGBA8_SNorm:
            case Image::Format::RGBA8_UNorm:
            case Image::Format::RGBA16_SNorm:
            case Image::Format::BGRA8_UNorm:
            case Image::Format::sRGB8:
            case Image::Format::sBGR8:
            case Image::Format::sRGBA8:
            case Image::Format::sBGRA8: return 1u;

            case Image::Format::R16_SNorm:
            case Image::Format::R16_UNorm:
            case Image::Format::R16I:
            case Image::Format::R16U:
            case Image::Format::RG16_SNorm:
            case Image::Format::RG16_UNorm:
            case Image::Format::RG16I:
            case Image::Format::RG16U:
            case Image::Format::RG16F:
            case Image::Format::RGB16I:
            case Image::Format::RGB16U:
            case Image::Format::RGB16F:
            case Image::Format::RGBA16I:
            case Image::Format::RGBA16U:
            case Image::Format::RGBA16F:
            case Image::Format::R16F: return 2u;

            case Image::Format::R32I:
            case Image::Format::R32U:
            case Image::Format::R32F:
            case Image::Format::RG32I:
            case Image::Format::RG32U:
            case Image::Format::RG32F:
            case Image::Format::RGB16_SNorm:
            case Image::Format::RGB32I:
            case Image::Format::RGB32U:
            case Image::Format::RGB32F:
            case Image::Format::RGBA8I:
            case Image::Format::RGBA8U:
            case Image::Format::RGBA32I:
            case Image::Format::RGBA32U:
            case Image::Format::RGBA32F: return 4u;

            default: return 0u;
        }

        return 0u;
    }
} // namespace storm::image
