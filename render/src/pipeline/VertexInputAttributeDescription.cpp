#include <stormkit/render/pipeline/VertexInputAttributeDescription.mpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<render::VertexInputAttributeDescription>::operator()(
        const render::VertexInputAttributeDescription &description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.location);
        core::hashCombine(hash, description.format);
        core::hashCombine(hash, description.offset);

        return hash;
    }
} // namespace std
