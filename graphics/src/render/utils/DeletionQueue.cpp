#include <storm/engine/render/utils/DeletionQueue.hpp>

#include <storm/render/core/CommandBuffer.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>
#include <storm/render/resource/TextureView.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
DeletionQueue::DeletionQueue(const Engine &engine) : m_engine { engine } {
}

/////////////////////////////////////
/////////////////////////////////////
DeletionQueue::~DeletionQueue() = default;

/////////////////////////////////////
/////////////////////////////////////
DeletionQueue::DeletionQueue(DeletionQueue &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto DeletionQueue::operator=(DeletionQueue &&) noexcept -> DeletionQueue & = default;

/////////////////////////////////////
/////////////////////////////////////
void DeletionQueue::flush() {
    /*const auto to_remove = core::ranges::remove_if(m_to_delete, [](auto &to_delete) {
        if (to_delete.fence->status() == render::Fence::Status::Signaled) {
            to_delete.func();
            return true;
        }

        return false;
    });

    m_to_delete.erase(to_remove.begin(), to_remove.end());*/

    auto to_remove =
        std::remove_if(core::ranges::begin(m_to_delete),
                       core::ranges::end(m_to_delete),
                       [](auto &to_delete) {
                           if (to_delete.fence->status() == render::Fence::Status::Signaled) {
                               to_delete.func();
                               return true;
                           }

                           return false;
                       });

    m_to_delete.erase(to_remove, core::ranges::end(m_to_delete));
}
