// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::core {
    using tl::swap;

    template<typename R, typename... Args>
    using FunctionRef = tl::function_ref<R(Args...)>;
} // namespace stormkit::core
