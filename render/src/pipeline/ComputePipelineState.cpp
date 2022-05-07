#include <storm/render/pipeline/ComputePipelineState.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<ComputePipelineShaderState>::operator()(
        [[maybe_unused]] const ComputePipelineShaderState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader);

        return 0;
    }

    core::Hash64
        hash<ComputePipelineLayout>::operator()(const ComputePipelineLayout &state) const noexcept {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto &layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    core::Hash64
        hash<ComputePipelineState>::operator()(const ComputePipelineState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader_state);
        core::hashCombine(hash, state.layout);

        return hash;
    }
} // namespace std
