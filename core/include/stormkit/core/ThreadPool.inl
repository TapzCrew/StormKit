// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "ThreadPool.mpp"

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ThreadPool::workerCount() const noexcept -> core::USize { return m_worker_count; }

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
} // namespace stormkit::core
