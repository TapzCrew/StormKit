// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto ThreadPool::workerCount() const noexcept -> core::USize {
        return m_worker_count;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    auto ThreadPool::postTask(Callback<T> callback) -> std::future<T> {
        return postTask(Task::Type::Standard, std::move(callback));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    auto ThreadPool::postTask(Callback<T> callback, NoFutureType t) -> void {
        postTask(Task::Type::Standard, std::move(callback), t);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    auto ThreadPool::postTask(Task::Type type, Callback<T> callback) -> std::future<T> {
        auto task = Task { .type = type, .work = callback };

        auto future = task.work.get_future();

        {
            auto lock = std::unique_lock { m_mutex };

            m_tasks.emplace(std::move(task));
        }

        m_work_signal.notify_one();

        return future;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    auto ThreadPool::postTask(Task::Type type, Callback<T> callback, NoFutureType) -> void {
        {
            auto lock = std::unique_lock { m_mutex };

            m_tasks.emplace(type, std::move(callback));
        }

        m_work_signal.notify_one();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::ranges::range Range, std::invocable<typename Range::element_type&> F>
    auto parallelFor(ThreadPool& pool, Range&& range, F&& f) -> std::future<void> {
        if constexpr (std::is_rvalue_reference_v<Range>)
            return pool.postTask<void>([range = std::move(range), f = std::forward<F>(f)]() {
                auto futures = core::transform(range, [](auto& elem) { return f(elem); });

                for (auto& future : futures) future.wait();
            });
        else
            return pool.postTask<void>([&range, f = std::forward<F>(f)]() {
                auto futures = core::transform(range, [](auto& elem) { return f(elem); });

                for (auto& future : futures) future.wait();
            });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::ranges::range Range, std::invocable<typename Range::element_type&> F>
    auto parallelTransform(ThreadPool& pool, Range&& range, F&& f)
        -> std::future<std::invoke_result_t<F, typename Range::element_type&>> {
        using Element    = typename Range::element_type;
        using OutputType = std::invoke_result_t<F, Element&>;

        if constexpr (std::is_rvalue_reference_v<Range>)
            return pool.postTask<std::vector<typename Range::element_type&>>(
                [range = std::move(range), f = std::forward<F>(f), &pool]() {
                    auto tasks = core::transform(range, [](auto& elem) { return f(elem); });

                    auto futures = core::transform(tasks, [&pool](auto& l) {
                        return pool.postTask<OutputType>(std::move(l));
                    });

                    return core::transform(futures, [](auto& future) { return future.get(); });
                });
        else
            return pool.postTask<std::vector<typename Range::element_type&>>(
                [&range, f = std::forward<F>(f), &pool]() {
                    auto tasks = core::transform(range, [](auto& elem) { return f(elem); });

                    auto futures = core::transform(tasks, [&pool](auto& l) {
                        return pool.postTask<OutputType>(std::move(l));
                    });

                    return core::transform(futures, [](auto& future) { return future.get(); });
                });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::ranges::range Range,
             std::predicate<typename Range::element_type&> Predicate,
             std::invocable<typename Range::element_type&> F>
    auto parallelTransformIf(ThreadPool& pool, Range&& range, Predicate&& predicate, F&& f)
        -> std::future<std::invoke_result_t<F, typename Range::element_type&>> {
        using Element    = typename Range::element_type;
        using OutputType = std::invoke_result_t<F, Element&>;

        if constexpr (std::is_rvalue_reference_v<Range>)
            return pool.postTask<std::vector<typename Range::element_type&>>(
                [range     = std::move(range),
                 f         = std::forward<F>(f),
                 predicate = std::forward<Predicate>(predicate),
                 &pool]() {
                    auto tasks =
                        core::transformIf(range, predicate, [](auto& elem) { return f(elem); });

                    auto futures = core::transform(tasks, [&pool](auto& l) {
                        return pool.postTask<OutputType>(std::move(l));
                    });

                    return core::transform(futures, [](auto& future) { return future.get(); });
                });
        else
            return pool.postTask<std::vector<typename Range::element_type&>>(
                [&range,
                 f         = std::forward<F>(f),
                 predicate = std::forward<Predicate>(predicate),
                 &pool]() {
                    auto tasks =
                        core::transformIf(range, predicate, [](auto& elem) { return f(elem); });

                    auto futures = core::transform(tasks, [&pool](auto& l) {
                        return pool.postTask<OutputType>(std::move(l));
                    });

                    return core::transform(futures, [](auto& future) { return future.get(); });
                });
    }
} // namespace stormkit::core
