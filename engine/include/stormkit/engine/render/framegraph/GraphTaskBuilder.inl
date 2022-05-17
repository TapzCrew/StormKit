// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "GraphTaskBuilder.mpp"

namespace stormkit::engine {
    template<ResourceDescriptionType Description>
    auto GraphTaskBuilder::create(std::string name, Description &&description)
        -> GraphResource<Description> & {
        const auto &resource = m_framegraph->m_resources.emplace_back(
            std::make_unique<GraphResource<Description>>(std::move(name),
                                                         m_task->id(),
                                                         std::forward<Description>(description)));

        m_task->m_creates.emplace_back(resource->id());

        return static_cast<GraphResource<Description> &>(*resource);
    }

    template<ResourceDescriptionType Description>
    auto GraphTaskBuilder::read(GraphResource<Description> &resource)
        -> GraphResource<Description> & {
        resource.m_readers.emplace_back(m_task->id());
        m_task->m_reads.emplace_back(resource.id());

        return resource;
    }

    template<ResourceDescriptionType Description>
    auto GraphTaskBuilder::write(GraphResource<Description> &resource)
        -> GraphResource<Description> & {
        resource.m_writers.emplace_back(m_task->id());
        m_task->m_writes.emplace_back(resource.id());

        return resource;
    }

    template<ResourceDescriptionType Description>
    auto GraphTaskBuilder::readwrite(GraphResource<Description> &resource)
        -> GraphResource<Description> & {
        [[maybe_unused]] const auto _1 = read(resource);
        [[maybe_unused]] const auto _2 = write(resource);

        return resource;
    }
} // namespace stormkit::engine
