// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/render/core/Surface.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    Surface::Surface(const Instance &instance, Buffering buffering)
        : m_instance { &instance }, m_buffering { buffering } {
        onSwapchainFenceSignaled = [](const auto &fence) {};
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Surface::~Surface() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Surface::Surface(Surface &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Surface::operator=(Surface &&) noexcept -> Surface & = default;
} // namespace stormkit::render
