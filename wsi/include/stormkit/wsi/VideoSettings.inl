#pragma once

namespace stormkit::wsi {
    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto VideoSettings::operator<=>(const VideoSettings &other) const noexcept
        -> std::strong_ordering {
        if (auto ret = size.width <=> other.size.width; ret != 0) return ret;
        if (auto ret = size.height <=> other.size.height; ret != 0) return ret;
        if (auto ret = bpp <=> other.bpp; ret != 0) return ret;
        if (auto ret = dpi <=> other.dpi; ret != 0) return ret;

        return std::strong_ordering::equal;
    }
} // namespace stormkit::wsi
