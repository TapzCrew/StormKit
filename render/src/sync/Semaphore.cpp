// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Semaphore::Semaphore(const render::Device &device) : m_device { &device } {
    const auto create_info = vk::SemaphoreCreateInfo {};

    m_vk_semaphore = m_device->createVkSemaphore(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Semaphore::~Semaphore() = default;

/////////////////////////////////////
/////////////////////////////////////
Semaphore::Semaphore(Semaphore &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Semaphore &Semaphore::operator=(Semaphore &&) = default;
