// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    Timer<Clock, Duration>::Timer() : m_stop(false) {
        ScopedLock lock(m_lock);
        m_worker = std::thread(std::bind(&Timer::workerTask, this));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    Timer<Clock, Duration>::~Timer() {
        {
            ScopedLock lock(m_lock);
            m_stop = true;
            m_wake_up.notify_all();
        }
        if (m_worker.joinable()) m_worker.join();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    typename Timer<Clock, Duration>::DeferredTaskId
        Timer<Clock, Duration>::makeTask(Duration when, Duration period, const Callback &callback) {
        return makeTask({ 0, Clock::now() + when, period, callback });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    typename Timer<Clock, Duration>::DeferredTaskId
        Timer<Clock, Duration>::makeTask(Duration when, Duration period, Callback &&callback) {
        return makeTask(DeferredTask(0, Clock::now() + when, period, std::move(callback)));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    void Timer<Clock, Duration>::deleteTask(Timer::DeferredTaskId id) {
        ScopedLock lock;
        auto it = m_tasks.find(id);
        if (it == m_tasks.end()) return;
        else if (it->second.running)
            it->second.running = false;
        else {
            m_task_queue.erase(std::ref(it->second));
            m_tasks.erase(it);
        }

        m_wake_up.notify_all();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    bool Timer<Clock, Duration>::existTask(DeferredTaskId id) {
        ScopedLock lock(m_lock);
        return m_tasks.find(id) != core::ranges::end(m_tasks);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    typename Timer<Clock, Duration>::DeferredTaskId
        Timer<Clock, Duration>::makeTask(DeferredTask &&task) {
        DeferredTaskId id;
        {
            ScopedLock lock(m_lock);
            id = task.id = getNextDeferredTaskId();

            auto it = m_tasks.emplace(id, std::move(task));
            m_task_queue.insert(it.first->second);
            m_wake_up.notify_all();
        }

        return id;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class Clock, class Duration>
    void Timer<Clock, Duration>::workerTask() {
        ScopedLock lock(m_lock);
        while (!m_stop) {
            if (m_task_queue.empty()) m_wake_up.wait(lock);
            else {
                auto task_ref_w    = m_task_queue.begin();
                DeferredTask &task = *task_ref_w;
                auto now           = Clock::now();

                if (now >= task.next) {
                    m_task_queue.erase(task_ref_w);

                    task.running = true;

                    lock.unlock();
                    task.callback();
                    lock.lock();

                    if (m_stop) break;
                    else if (!task.running)
                        m_tasks.erase(task.id);
                    else {
                        task.running = false;

                        if (task.duration.count() > 0) {
                            task.next = task.next + task.duration;
                            m_task_queue.insert(task);
                        } else
                            m_tasks.erase(task.id);
                    }
                } else
                    m_wake_up.wait_until(lock, task.next);
            }
        }
    }
} // namespace storm::core
