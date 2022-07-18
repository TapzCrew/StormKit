// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <iterator>

namespace stormkit::core {
    template<std::ranges::range auto... Ranges>
    class MultidimentionalIterator {
        using tuple_type = std::tuple<typename decltype(Ranges)::value_type &...>;

      public:
        using iterator_category = std::input_iterator_tag;

        using difference_type = std::ptrdiff_t;
        using value_type      = tuple_type;
        using reference       = tuple_type &;
        using pointer         = tuple_type *;

        auto operator++() -> MultidimentionalIterator &;

        auto operator++(int) -> void;

        auto operator*() const noexcept -> reference;
        auto operator->() const noexcept -> pointer;

      private:
    };
} // namespace stormkit::core

#include "Iterator.inl"
