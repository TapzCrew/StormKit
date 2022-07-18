#include <stormkit/gpu/pipeline/VertexBindingDescription.hpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::VertexBindingDescription>::operator()(
        const gpu::VertexBindingDescription &description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.stride);
        core::hashCombine(hash, description.input_rate);

        return hash;
    }
} // namespace std
