#include <storm/render/core/Types.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<Viewport>::operator()(const Viewport &viewport) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, viewport.depth);
        core::hashCombine(hash, viewport.extent);
        core::hashCombine(hash, viewport.position);

        return hash;
    }

    core::Hash64 hash<Scissor>::operator()(const Scissor &scissor) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, scissor.offset);
        core::hashCombine(hash, scissor.extent);

        return hash;
    }
} // namespace std
