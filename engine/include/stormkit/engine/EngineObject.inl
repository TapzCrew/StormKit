// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::engine {
    inline EngineObject::EngineObject(Engine &engine) noexcept : m_engine { &engine } {}
    inline EngineObject::~EngineObject() noexcept = default;

    inline EngineObject::EngineObject(const EngineObject &) noexcept = default;
    inline auto EngineObject::operator=(const EngineObject &) noexcept -> EngineObject & = default;

    inline EngineObject::EngineObject(EngineObject &&) noexcept = default;
    inline auto EngineObject::operator=(EngineObject &&) noexcept -> EngineObject & = default;

    inline auto EngineObject::engine() noexcept -> Engine & {
        STORMKIT_EXPECTS(m_engine);

        return *m_engine;
    }
    inline auto EngineObject::engine() const noexcept -> const Engine & {
        STORMKIT_EXPECTS(m_engine);

        return *m_engine;
    }
} // namespace stormkit::engine
