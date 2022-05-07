// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    template<details::FrameGraphDescriptorConcept Descriptor>
    auto FrameGraphBuilder::create(std::string name, Descriptor descriptor)
        -> FrameGraphResourceID {
        auto &frame_graph = m_frame_graph.get();
        auto &frame_pass  = m_frame_pass.get();

        auto id               = frame_graph.createTransientResource(std::move(name), descriptor);
        auto &resource        = frame_graph.getResource(id);
        resource.m_created_by = frame_pass.id();
        resource.m_writed_in.emplace_back(frame_pass.id());

        frame_pass.m_creates.emplace_back(id);
        frame_pass.m_writes.emplace_back(id);

        return id;
    }
} // namespace storm::engine
