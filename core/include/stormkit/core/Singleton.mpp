// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <utility>

namespace stormkit::core {
    template<class T>
    class Singleton {
      public:
        template<class... Args>
        static auto instance(Args &&...args) -> T &;

        Singleton(Singleton &&)      = delete;
        Singleton(const Singleton &) = delete;

        auto operator=(Singleton &&) -> Singleton & = delete;
        auto operator=(const Singleton &) -> Singleton & = delete;

      protected:
        Singleton() noexcept  = default;
        ~Singleton() noexcept = default;

      private:
        static auto onceFlag() -> std::once_flag &;

        static inline std::unique_ptr<T> m_instance = nullptr;
    };
} // namespace stormkit::core

#include "Singleton.inl"
