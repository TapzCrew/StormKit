#include <stormkit/render/pipeline/VertexBindingDescription.mpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<render::VertexBindingDescription>::operator()(
        const render::VertexBindingDescription &description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.stride);
        core::hashCombine(hash, description.input_rate);

        return hash;
    }
} // namespace std
