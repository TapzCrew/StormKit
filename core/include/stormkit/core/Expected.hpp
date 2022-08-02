// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
    #include <expected>
namespace stormkit::core {
    template<class T, class E>
    using Expected = std::expected<T, E>;

    template<class E>
    using Unexpected = std::unexpected<E>;

    template<typename E>
    auto make_unexpected(E&& e) {
        return Unexpected<std::remove_cvref_t<E>> { std::forward<E>(e) };
    }

    template<typename E>
    auto makeUnexpected(E&& e) {
        return make_unexpected(std::forward<E>(e));
    }
} // namespace stormkit::core
#else
    #include <stormkit/core/details/expected.hpp>

namespace tl {
    template<class E>
    unexpected(E e) -> unexpected<E>;
}

namespace stormkit::core {
    template<class T, class E>
    using Expected = tl::expected<T, E>;

    template<class E>
    using Unexpected = tl::unexpected<E>;

    using tl::make_unexpected;

    template<typename E>
    auto makeUnexpected(E&& e) {
        return make_unexpected(std::forward<E>(e));
    }
} // namespace stormkit::core

#endif
