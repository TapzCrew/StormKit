// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/gpu/core/Surface.mpp>

namespace stormkit::gpu {
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
} // namespace stormkit::gpu
