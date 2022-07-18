#include <stormkit/gpu/pipeline/ComputePipelineState.hpp>
#include <stormkit/gpu/pipeline/DescriptorSetLayout.hpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<gpu::ComputePipelineShaderState>::operator()(
        [[maybe_unused]] const gpu::ComputePipelineShaderState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader);

        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<gpu::ComputePipelineLayout>::operator()(
        const gpu::ComputePipelineLayout &state) const noexcept {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto &layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<gpu::ComputePipelineState>::operator()(
        const gpu::ComputePipelineState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader_state);
        core::hashCombine(hash, state.layout);

        return hash;
    }
} // namespace std
