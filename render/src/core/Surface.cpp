// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Surface::Surface(const render::Instance &instance, Buffering buffering)
    : m_instance { &instance }, m_buffering { buffering } {
    onSwapchainFenceSignaled = [](const auto &fence) {};
};

/////////////////////////////////////
/////////////////////////////////////
Surface::~Surface() = default;

/////////////////////////////////////
/////////////////////////////////////
Surface::Surface(Surface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Surface &Surface::operator=(Surface &&) = default;
