// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Surface.hpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Surface::Surface(const Instance &instance, Buffering buffering)
        : InstanceObject { instance }, m_buffering { buffering } {
        onSwapchainFenceSignaled = []([[maybe_unused]] const auto &fence) {};
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
