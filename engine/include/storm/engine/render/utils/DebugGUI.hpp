// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::render - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC DebugGUI: public core::NonCopyable {
      public:
        DebugGUI(Engine &engine, window::Keyboard &keyboard, window::Mouse &mouse);
        ~DebugGUI();

        DebugGUI(DebugGUI &&) noexcept;
        DebugGUI &operator=(DebugGUI &&) noexcept;

        void init(const render::RenderPass &render_pass);

        void update();
        void render(render::CommandBuffer &cmb, const render::RenderPass &render_pass);

        void setSkipFrameCount(core::UInt32 count) noexcept;

        ENGINE_GETTERS
      private:
        engine::EngineRef m_engine;
        window::KeyboardRef m_keyboard;
        window::MouseRef m_mouse;

        render::CommandBufferOwnedPtrArray m_sub_command_buffers;

        core::UInt32 m_buffer_count;
        core::UInt32 m_current_buffer = 0;

        float m_current_cpu_time = 0.f;
        std::array<float, 40> m_frame_times;
        core::UInt8 m_frame_time_pointer = 0u;
        core::UInt32 m_max_fps           = 300u;

        core::UInt32 m_last_entry_index   = 0u;
        core::UInt32 m_skip_frame         = 0u;
        core::UInt32 m_skip_frame_counter = m_skip_frame;
    };
} // namespace storm::engine

#include "DebugGUI.inl"
