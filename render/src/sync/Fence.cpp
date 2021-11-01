// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/sync/Fence.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Fence::Fence(const render::Device &device) : m_device { &device } {
    const auto create_info = vk::FenceCreateInfo {}.setFlags(vk::FenceCreateFlagBits::eSignaled);

    m_vk_fence = m_device->createVkFence(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Fence::~Fence() = default;

/////////////////////////////////////
/////////////////////////////////////
Fence::Fence(Fence &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Fence &Fence::operator=(Fence &&) = default;

/////////////////////////////////////
/////////////////////////////////////
render::WaitResult Fence::wait(uint64_t wait_for) const {
    auto result = m_device->waitForVkFence(*m_vk_fence, wait_for);

    if (result == vk::Result::eSuccess) return render::WaitResult::Success;
    else if (result == vk::Result::eTimeout)
        return render::WaitResult::Timeout;

    return render::WaitResult::Error;
}

/////////////////////////////////////
/////////////////////////////////////
void Fence::reset() {
    m_device->resetVkFence(*m_vk_fence);
}

/////////////////////////////////////
/////////////////////////////////////
Fence::Status Fence::status() const noexcept {
    auto status = m_device->getVkFenceStatus(*m_vk_fence);

    return (status == vk::Result::eSuccess) ? Status::Signaled : Status::Unsignaled;
}
