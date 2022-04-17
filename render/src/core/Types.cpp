#include <stormkit/render/core/Types.mpp>

#include <stormkit/render/pipeline/DescriptorSet.mpp>

using namespace stormkit;

namespace std {
    auto hash<render::Viewport>::operator()(const render::Viewport &viewport) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, viewport.depth);
        core::hashCombine(hash, viewport.extent);
        core::hashCombine(hash, viewport.position);

        return hash;
    }

    auto hash<render::Scissor>::operator()(const render::Scissor &scissor) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, scissor.offset);
        core::hashCombine(hash, scissor.extent);

        return hash;
    }

    auto hash<render::BufferDescriptor>::operator()(
        const render::BufferDescriptor &descriptor) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.buffer);
        core::hashCombine(hash, descriptor.range);
        core::hashCombine(hash, descriptor.offset);

        return hash;
    }

    auto hash<render::ImageDescriptor>::operator()(
        const render::ImageDescriptor &descriptor) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.layout);
        core::hashCombine(hash, descriptor.image_view);
        core::hashCombine(hash, descriptor.sampler);

        return hash;
    }

    auto hash<render::Descriptor>::operator()(
        const render::Descriptor &descriptor_proxy) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        std::visit(core::Overload { [&hash](auto &descriptor) {
                       core::hashCombine(hash, descriptor);
                   } },
                   descriptor_proxy);

        return hash;
    }
} // namespace std
