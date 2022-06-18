// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::image {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::pixel(core::USize index,
                             core::UInt32 layer,
                             core::UInt32 face,
                             core::UInt32 level) noexcept -> core::ByteSpan {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        auto _data = data(layer, face, level);

        STORMKIT_EXPECTS(index < m_data.extent.width * m_data.extent.height * m_data.extent.depth);

        const auto block_size = m_data.channel_count * m_data.bytes_per_channel;

        return { std::data(_data) + index * block_size, block_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::pixel(core::USize index,
                             core::UInt32 layer,
                             core::UInt32 face,
                             core::UInt32 level) const noexcept -> core::ByteConstSpan {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        auto _data = data(layer, face, level);

        const auto mip_extent = extent(level);
        STORMKIT_EXPECTS(index < mip_extent.width * mip_extent.height * mip_extent.depth);

        const auto block_size = m_data.channel_count * m_data.bytes_per_channel;

        return { std::data(_data) + index * block_size, block_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::pixel(core::Position3u position,
                             core::UInt32 layer,
                             core::UInt32 face,
                             core::UInt32 level) noexcept -> core::ByteSpan {
        const auto mip_extent = extent(level);

        const auto id = position->x + (position->y * mip_extent.width) +
                        (mip_extent.width * mip_extent.height * position->z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::pixel(core::Position3u position,
                             core::UInt32 layer,
                             core::UInt32 face,
                             core::UInt32 level) const noexcept -> core::ByteConstSpan {
        const auto mip_extent = extent(level);

        const auto id = position->x + (position->y * mip_extent.width) +
                        (mip_extent.width * mip_extent.height * position->z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::extent(core::UInt32 level) const noexcept -> core::ExtentU {
        STORMKIT_EXPECTS(m_data.mip_levels > level);

        return { std::max(1u, m_data.extent.width >> level),
                 std::max(1u, m_data.extent.height >> level),
                 std::max(1u, m_data.extent.depth >> level) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::channelCount() const noexcept -> core::UInt32 {
        return m_data.channel_count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::bytesPerChannel() const noexcept -> core::UInt32 {
        return m_data.bytes_per_channel;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::layers() const noexcept -> core::UInt32 { return m_data.layers; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::faces() const noexcept -> core::UInt32 { return m_data.faces; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::mipLevels() const noexcept -> core::UInt32 { return m_data.mip_levels; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::format() const noexcept -> Format { return m_data.format; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::size() const noexcept -> core::USize { return std::size(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::size(core::UInt32 layer,
                            core::UInt32 face,
                            core::UInt32 level) const noexcept -> core::USize {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        const auto mip_extent = extent(level);

        return mip_extent.width * mip_extent.height * mip_extent.depth * m_data.channel_count *
               m_data.bytes_per_channel;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::size(core::UInt32 layer, core::UInt32 face) const noexcept -> core::USize {
        auto _size = core::USize { 0u };
        for (auto i : core::range(m_data.mip_levels)) _size += size(layer, face, i);

        return _size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::size(core::UInt32 layer) const noexcept -> core::USize {
        auto _size = core::USize { 0u };
        for (auto i : core::range(m_data.faces)) _size += size(layer, i);

        return _size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::data() noexcept -> core::ByteSpan { return m_data.data; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept
        -> core::ByteSpan {
        const auto mip_size = size(layer, face, level);

        auto offset = std::size_t { 0 };

        for (auto i : core::range(layer)) offset += size(i);

        for (auto j : core::range(face)) offset += size(layer, j);

        for (auto k : core::range(level)) offset += size(layer, face, k);

        return { std::data(m_data.data) + offset, mip_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::data() const noexcept -> core::ByteConstSpan { return m_data.data; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::data(core::UInt32 layer,
                            core::UInt32 face,
                            core::UInt32 level) const noexcept -> core::ByteConstSpan {
        const auto mip_size = size(layer, face, level);

        auto offset = std::size_t { 0 };

        for (auto i : core::range(layer)) offset += size(i);

        for (auto j : core::range(face)) offset += size(layer, j);

        for (auto k : core::range(level)) offset += size(layer, face, k);

        return { std::data(m_data.data) + offset, mip_size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::begin() noexcept { return std::begin(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        return std::begin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::begin() const noexcept { return std::begin(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto
        Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return std::begin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::cbegin() const noexcept { return std::cbegin(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto
        Image::cbegin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        return std::cbegin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::end() noexcept { return std::end(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        return std::end(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::end() const noexcept { return std::end(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto
        Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        return std::end(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::cend() const noexcept { return std::cend(m_data.data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto
        Image::cend(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORMKIT_EXPECTS(m_data.mip_levels > level);
        STORMKIT_EXPECTS(m_data.faces > face);
        STORMKIT_EXPECTS(m_data.layers > layer);

        return std::cend(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::imageData() const noexcept -> const ImageData & { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto getChannelCountFor(Image::Format format) noexcept -> core::UInt8 {
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

            default: break;
        }

        return 0u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto getSizeof(Image::Format format) noexcept -> core::UInt8 {
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

            default: break;
        }

        return 0u;
    }
} // namespace stormkit::image
