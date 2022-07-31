#pragma once

namespace stormkit::wsi {
    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto Monitor::operator<=>(const Monitor& other) const noexcept
        -> std::strong_ordering {
        if (auto ret = flags <=> other.flags; ret != 0) return ret;

#ifdef STORMKIT_COMPILER_CLANG
        if (auto ret = name.compare(other.name); ret != 0) {
            if (ret < 0) return std::strong_ordering::less;
            else if (ret > 0)
                return std::strong_ordering::greater;
        }
#else
        if (auto ret = name <=> other.name; ret != 0) return ret;
#endif

        if (auto ret = std::size(extents) <=> std::size(other.extents); ret != 0) return ret;

        for (auto i : core::range(std::size(extents))) {
            if (auto ret = extents[i].width <=> other.extents[i].width; ret != 0) return ret;
            if (auto ret = extents[i].height <=> other.extents[i].height; ret != 0) return ret;
        }

        return std::strong_ordering::equal;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] constexpr auto Monitor::operator==(const Monitor& other) const noexcept -> bool {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto toString(Monitor::Flags flags) -> std::string {
        switch (flags) {
            case Monitor::Flags::None: return "Flags::None";
            case Monitor::Flags::Primary: return "Flags::Primary";
            default: break;
        }

        return "Unknown";
    }
} // namespace stormkit::wsi
