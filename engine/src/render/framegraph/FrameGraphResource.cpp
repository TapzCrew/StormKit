#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

#include <storm/engine/Engine.hpp>
#include <storm/engine/Profiler.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResource::FrameGraphResource(std::string name,
                                       FrameGraphResourceID id,
                                       bool transient,
                                       Descriptor descriptor) noexcept
    : m_id { id }, m_name { std::move(name) }, m_is_transient { transient }, m_descriptor {
          std::move(descriptor)
      } {
    if (std::holds_alternative<TextureDescriptor>(m_descriptor)) m_type = Type::Texture;
    else if (std::holds_alternative<BufferDescriptor>(m_descriptor))
        m_type = Type::Buffer;
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResource::~FrameGraphResource() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResource::FrameGraphResource(FrameGraphResource &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto FrameGraphResource::operator=(FrameGraphResource &&) noexcept
    -> FrameGraphResource &      = default;
