// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    template<class... Args>
    inline auto Singleton<T>::instance(Args &&...args) -> T & {
        auto lambdas = [](Args &&...args) mutable {
            m_instance = std::make_unique<T>(std::forward<Args>(args)...);
        };

        std::call_once(onceFlag(), lambdas, std::forward<Args>(args)...);

        return *m_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    inline auto Singleton<T>::onceFlag() -> std::once_flag & {
        static auto once_flag = std::once_flag {};
        return once_flag;
    }
} // namespace stormkit::core
