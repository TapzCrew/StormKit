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

        auto &task = m_tasks.emplace_back(
            std::make_unique<GraphTask<TaskData>>(std::move(name), std::forward<Args>(args)...));
        prepareTask(*task);

        return static_cast<GraphTask<TaskData> &>(*task);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType Description, gpu::ResourceType Resource>
    auto FrameGraphBuilder::setRetainedResource(std::string name,
                                                Description &&description,
                                                const Resource &image)
        -> GraphResource<Description> & {
        auto &graph_resource = m_resources.emplace_back(
            std::make_unique<GraphResource<Description>>(std::move(name),
                                                         INVALID_ID,
                                                         std::forward<Description>(description)));

        m_retained_resources.emplace(graph_resource->id(), ResourceVariant { &image });

        return static_cast<GraphResource<Description> &>(*graph_resource);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::setFinalResource(GraphID id) noexcept -> void {
        m_final_resource = id;
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

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::hasResource(GraphID id) const noexcept -> bool {
        return std::ranges::any_of(m_resources, [&](const auto &t) { return t->id() == id; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::hasResource(std::string_view name) const noexcept -> bool {
        return std::ranges::any_of(m_resources, [&](const auto &t) { return t->name() == name; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename ResourceData>
    auto FrameGraphBuilder::getResource(std::string_view name) noexcept
        -> GraphResource<ResourceData> & {
        return static_cast<GraphResource<ResourceData> &>(getResource(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename ResourceData>
    auto FrameGraphBuilder::getResource(std::string_view name) const noexcept
        -> const GraphResource<ResourceData> & {
        return static_cast<const GraphResource<ResourceData> &>(getResource(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename ResourceData>
    auto FrameGraphBuilder::getResource(GraphID id) noexcept -> GraphResource<ResourceData> & {
        return static_cast<GraphResource<ResourceData> &>(getResource(id));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename ResourceData>
    auto FrameGraphBuilder::getResource(GraphID id) const noexcept
        -> const GraphResource<ResourceData> & {
        return static_cast<const GraphResource<ResourceData> &>(getResource(id));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getResource(std::string_view name) noexcept
        -> GraphResourceBase & {
        auto it =
            std::ranges::find_if(m_resources, [&](const auto &t) { return t->name() == name; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::end(m_resources),
                                 core::format("resource with name {} not found", name));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getResource(std::string_view name) const noexcept
        -> const GraphResourceBase & {
        const auto it =
            std::ranges::find_if(m_resources, [&](const auto &t) { return t->name() == name; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::cend(m_resources),
                                 core::format("resource with name {} not found", name));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getResource(GraphID id) noexcept -> GraphResourceBase & {
        auto it = std::ranges::find_if(m_resources, [&](const auto &t) { return t->id() == id; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::end(m_resources),
                                 core::format("resource with id {} not found", id));

        return **it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphBuilder::getResource(GraphID id) const noexcept
        -> const GraphResourceBase & {
        const auto it =
            std::ranges::find_if(m_resources, [&](const auto &t) { return t->id() == id; });

        STORMKIT_ENSURES_MESSAGE(it != std::ranges::cend(m_resources),
                                 core::format("resource with id {} not found", id));

        return **it;
    }
} // namespace stormkit::engine
