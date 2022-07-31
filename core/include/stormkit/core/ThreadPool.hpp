// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <future>
#include <queue>
#include <thread>
#include <vector>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    class STORMKIT_PUBLIC ThreadPool {
      public:
        static inline struct NoFutureType {
        } NoFuture;

        template<typename T>
        using Callback = std::function<T()>;

        ThreadPool(core::USize worker_count = std::thread::hardware_concurrency() / 2);
        ~ThreadPool();

        ThreadPool(const ThreadPool &) = delete;
        auto operator=(const ThreadPool &) -> ThreadPool & = delete;

        ThreadPool(ThreadPool &&) noexcept;
        auto operator=(ThreadPool &&) noexcept -> ThreadPool &;

        auto workerCount() const noexcept -> core::USize;

        template<typename T>
        auto postTask(Callback<T> callback) -> std::future<T>;
        template<typename T>
        auto postTask(Callback<T> callback, NoFutureType) -> void;

        auto joinAll() -> void;

      private:
        struct Task {
            enum class Type { Standard, Terminate };

            Task() = default;
            Task(Type _type, Callback<void> _work) : type { _type }, work { std::move(_work) } {}

            Type type;
            std::packaged_task<void()> work;
        };

        template<typename T>
        auto postTask(Task::Type type, Callback<T> callback) -> std::future<T>;
        template<typename T>
        auto postTask(Task::Type type, Callback<T> callback, NoFutureType) -> void;

        auto workerMain() noexcept -> void;

        core::USize m_worker_count = 0;

        std::vector<std::thread> m_workers;

        mutable std::mutex m_mutex;
        std::condition_variable m_work_signal;
        std::queue<Task> m_tasks;
    };

    template<std::ranges::range Range, std::invocable<typename Range::element_type &> F>
    auto parallelFor(ThreadPool &pool, Range &&range, F &&f)
        -> std::future<std::invoke_result_t<F, typename Range::element_type &>>;

    template<std::ranges::range Range, std::invocable<typename Range::element_type &> F>
    auto parallelTransform(ThreadPool &pool, Range &&range, F &&f)
        -> std::future<std::invoke_result_t<F, typename Range::element_type &>>;

    template<std::ranges::range Range,
             std::predicate<typename Range::element_type &> Predicate,
             std::invocable<typename Range::element_type &> F>
    auto parallelTransformIf(ThreadPool &pool, Range &&range, Predicate &&predicate, F &&f)
        -> std::future<std::invoke_result_t<F, typename Range::element_type &>>;
} // namespace stormkit::core

#include "ThreadPool.inl"