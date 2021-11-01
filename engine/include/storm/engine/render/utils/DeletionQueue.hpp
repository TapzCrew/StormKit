// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC DeletionQueue: public core::NonCopyable {
      public:
        using DeleteFunc = std::function<void()>;

        explicit DeletionQueue(const Engine &engine);
        ~DeletionQueue();

        DeletionQueue(DeletionQueue &&) noexcept;
        DeletionQueue &operator=(DeletionQueue &&) noexcept;

        void push(render::Fence &fence, DeleteFunc &&func);

        void flush();

        ENGINE_CONST_GETTER
      private:
        EngineConstRef m_engine;

        struct ToDelete {
            render::FencePtr fence;
            DeleteFunc func;
        };

        std::vector<ToDelete> m_to_delete;
    };
} // namespace storm::engine

#include "DeletionQueue.inl"
