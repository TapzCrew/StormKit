// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Engine;

import std;

import :Core.Application;

import stormkit.Core;
import stormkit.Wsi;
import stormkit.Entities;

import :Systems.RenderSystem;

namespace stormkit::engine {
    Application::Application(std::string_view application_name,
                             const core::math::ExtentU& window_extent,
                             std::string window_title,
                             PrivateTag) {
        m_renderer = Renderer::create(application_name)
                         .transform_error(core::expectsWithMessage("Failed to initialize renderer"))
                         .value();

        m_window = wsi::Window { std::move(window_title), window_extent, wsi::WindowStyle::Close };
        m_event_handler = wsi::EventHandler {};
        m_world         = entities::EntityManager {};

        m_world->addSystem<RenderSystem>();
    }

    auto Application::update() -> void {
        while (m_window->isOpen()) {
            m_event_handler->update(m_window);
            m_world->step(core::Secondf { 0 });
            // if (m_surf0ace->needRecreate()) {
            // m_surface->recreate();
            // doInitPerFrameObjects();
            //}

            // auto frame       = std::move(m_surface->acquireNextFrame().value());
            // auto& frame_data = m_frame_datas[frame.image_index];

            // auto wait   = core::makeConstObserverStaticArray(frame.image_available);
            // auto signal = core::makeConstObserverStaticArray(frame.render_finished);

            // frame_data.commandbuffer.submit(wait, signal, frame.in_flight);

            // m_surface->present(frame);
        }
    }
} // namespace stormkit::engine
