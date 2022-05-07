// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <storm/core/Memory.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>
#include <utility>

namespace storm::core {
    template<class T>
    class Singleton: public NonMovable, public NonCopyable {
      public:
        template<typename... Args>
        static T &instance(Args &&...args) {
            auto lambdas = [](Args &&...args) mutable {
                m_instance = std::make_unique<T>(std::forward<Args>(args)...);
            };

            std::call_once(onceFlag(), lambdas, std::forward<Args>(args)...);

            return *m_instance;
        }

      protected:
        explicit Singleton() = default;
        ~Singleton()         = default;

      private:
        static std::once_flag &onceFlag() {
            static auto once_flag = std::once_flag {};
            return once_flag;
        }
        static inline std::unique_ptr<T> m_instance = nullptr;
    };
} // namespace storm::core
