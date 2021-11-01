// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/System.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC RenderSystem: public entities::System {
      public:
        RenderSystem(Engine &engine, entities::EntityManager &manager, ComponentTypes types);
        ~RenderSystem();

        RenderSystem(RenderSystem &&) noexcept;
        RenderSystem &operator=(RenderSystem &&) noexcept;

        void render(render::Surface::Frame &frame);

        void setCamera(const Camera &camera);
        virtual void resetCamera() = 0;

        ENGINE_GETTERS
      protected:
        void onMessageReceived(const entities::Message &message) override;
        virtual void setupFrameGraph(FrameGraph &framegraph, FrameGraphResourceID backbuffer) = 0;

        EngineRef m_engine;

        using Entities = std::vector<entities::Entity>;
        Entities m_to_be_added;
        Entities m_to_be_removed;
        Entities m_entities;

        CameraConstPtr m_camera = nullptr;
        bool m_camera_switched  = true;

      private:
        void addEntity(entities::Entity e);
        void removeEntity(entities::Entity e);

        bool m_need_update = true;
    };
} // namespace storm::engine

#include "RenderSystem.inl"
