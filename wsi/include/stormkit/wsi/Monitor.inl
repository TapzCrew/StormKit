#pragma once

namespace stormkit::wsi {
    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto Monitor::operator<=>(const Monitor& other) const noexcept
        -> std::strong_ordering {
        if (auto ret = flags <=> other.flags; ret != 0) return ret;

#ifdef STORMKIT_OS_MACOS
        if (auto ret = name.compare(other.name); ret != 0) {
            if (ret < 0) return std::strong_ordering::less;
            else if (ret > 0)
                return std::strong_ordering::greater;
        }
#else
        if (auto ret = name <=> other.name; ret != 0) return ret;
#endif

        if (auto ret = std::size(sizes) <=> std::size(other.sizes); ret != 0) return ret;

        for (auto i : core::range(std::size(sizes))) {
            if (auto ret = sizes[i].width <=> other.sizes[i].width; ret != 0) return ret;
            if (auto ret = sizes[i].height <=> other.sizes[i].height; ret != 0) return ret;
        }

        return std::strong_ordering::equal;
    }

    [[nodiscard]] constexpr auto Monitor::operator==(const Monitor& other) const noexcept -> bool {
        return (*this <=> other) == std::strong_ordering::equal;
    }
} // namespace stormkit::wsi
