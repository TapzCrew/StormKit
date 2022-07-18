
#include <stormkit/core/Numerics.hpp>
#include <stormkit/core/ThreadPool.hpp>
#include <stormkit/core/ThreadUtils.hpp>

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    ThreadPool::ThreadPool(core::USize worker_count) : m_worker_count { worker_count } {
        m_workers.reserve(m_worker_count);
        for (auto i : range(m_worker_count)) {
            auto &thread = m_workers.emplace_back([this] { workerMain(); });
            core::setThreadName(thread, core::format("StormKit:WorkerThread:{}", i));
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ThreadPool::~ThreadPool() { joinAll(); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ThreadPool::ThreadPool(ThreadPool &&other) noexcept {
        auto lock = std::unique_lock { other.m_mutex };

        m_worker_count = std::exchange(other.m_worker_count, 0u);
        m_tasks        = std::move(other.m_tasks);

        m_workers.reserve(m_worker_count);
        for (auto i : range(m_worker_count)) {
            auto &thread = m_workers.emplace_back([this] { workerMain(); });
            core::setThreadName(thread, core::format("StormKit:WorkerThread:{}", i));
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ThreadPool::operator=(ThreadPool &&other) noexcept -> ThreadPool & {
        if (&other == this) [[unlikely]]
            return *this;

        auto lock1 = std::unique_lock { other.m_mutex, std::defer_lock };
        auto lock2 = std::unique_lock { other.m_mutex, std::defer_lock };
        std::lock(lock1, lock2);

        joinAll();

        m_worker_count = std::exchange(other.m_worker_count, 0u);
        m_tasks        = std::move(other.m_tasks);

        m_workers.reserve(m_worker_count);
        for (auto i : range(m_worker_count)) {
            auto &thread = m_workers.emplace_back([this] { workerMain(); });
            core::setThreadName(thread, core::format("StormKit:WorkerThread:{}", i));
        }

        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ThreadPool::joinAll() -> void {
        for ([[maybe_unused]] auto i : range(m_worker_count))
            postTask<void>(Task::Type::Terminate, {}, ThreadPool::NoFuture);

        for (auto &thread : m_workers) {
            if (thread.joinable()) thread.join();
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ThreadPool::workerMain() noexcept -> void {
        while (true) {
            auto task = Task {};

            {
                auto lock = std::unique_lock { m_mutex };
                if (std::empty(m_tasks))
                    m_work_signal.wait(lock, [this] { return !std::empty(m_tasks); });

                task = std::move(m_tasks.front());
                m_tasks.pop();
            }

            task.work();

            if (task.type == Task::Type::Terminate) return;
        }
    }
} // namespace stormkit::core
