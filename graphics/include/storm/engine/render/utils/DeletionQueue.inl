// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DeletionQueue::push(render::Fence &fence, DeleteFunc &&func) -> void {
        m_to_delete.emplace_back(ToDelete { core::makeObserver(fence), std::move(func) });
    }
} // namespace storm::engine
