// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <utility>

/////////// - StormKit::core - ///////////
#include <storm/core/Concepts.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    /// \brief `ConstexprMap` is a key/value container that can be initialized and used at compile
    /// time.
    /// \requires Type `Key` need to have an operator== defined.
    /// \unique_name ConstexprMap<Key, Value, Size>
    template<EQUALITY_COMPARABLE_CONCEPT(Key), typename Value, core::ArraySize Size>
    struct ConstexprMap {
        /// \exclude
        using ArrayType = std::array<std::pair<Key, Value>, Size>;

        /// \exclude
        ArrayType data;

        /// \brief Access specified element with bounds checking.
        ///
        /// Returns the mapped value of the element with `key` equivalent to key. If no such element
        /// / exists, an exception of type `std::out_of_range` is thrown (and at compile time, a
        /// compile time error).
        ///
        /// \returns The mapped value to `key`.
        [[nodiscard]] constexpr Value at(const Key &key) const {
            auto itr = ranges::find_if(data, [&key](const auto &v) { return v.first == key; });

            if (itr != end(data)) return itr->second;
            else
                throw std::out_of_range("Not Found");
        }
    };
} // namespace storm::core
