// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "FrameGraphBuilder.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData, typename... Args>
    auto FrameGraphBuilder::addTask(std::string name, Args &&...args) -> GraphTask<TaskData> & {
        STORMKIT_EXPECTS_MESSAGE(!hasTask(name),
                                 core::format("Task {} already registered !", name));

        auto &task = m_tasks.emplace_back(std::move(name), std::forward<Args>(args)...);
        prepareTask(task);

        return static_cast<GraphTask<TaskData> &>(task);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::hasTask(GraphID id) const noexcept -> bool {
        return std::ranges::any_of(m_tasks, [&](const auto &t) { return t->id() == id; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::hasTask(std::string_view name) const noexcept -> bool {
        return std::ranges::any_of(m_tasks, [&](const auto &t) { return t->name() == name; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto FrameGraphBuilder::getTask(std::string_view name) noexcept -> GraphTask<TaskData> & {
        return static_cast<GraphTask<TaskData> &>(getTask(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto FrameGraphBuilder::getTask(std::string_view name) const noexcept
        -> const GraphTask<TaskData> & {
        return static_cast<const GraphTask<TaskData> &>(getTask(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto FrameGraphBuilder::getTask(GraphID id) noexcept -> GraphTask<TaskData> & {
        return static_cast<GraphTask<TaskData> &>(getTask(id));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto FrameGraphBuilder::getTask(GraphID id) const noexcept -> const GraphTask<TaskData> & {
        return static_cast<const GraphTask<TaskData> &>(getTask(id));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getTask(std::string_view name) noexcept -> GraphTaskBase & {
        auto it = std::ranges::find_if(m_tasks, [&](const auto &t) { return t->name() == name; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::end(m_tasks),
                                 core::format("task with name {} not found", name));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getTask(std::string_view name) const noexcept
        -> const GraphTaskBase & {
        const auto it =
            std::ranges::find_if(m_tasks, [&](const auto &t) { return t->name() == name; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::cend(m_tasks),
                                 core::format("task with name {} not found", name));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getTask(GraphID id) noexcept -> GraphTaskBase & {
        auto it = std::ranges::find_if(m_tasks, [&](const auto &t) { return t->id() == id; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::end(m_tasks),
                                 core::format("task with id {} not found", id));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getTask(GraphID id) const noexcept -> const GraphTaskBase & {
        const auto it = std::ranges::find_if(m_tasks, [&](const auto &t) { return t->id() == id; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::cend(m_tasks),
                                 core::format("task with id {} not found", id));

        return **it;
    }
} // namespace stormkit::engine
