#include <stormkit/render/pipeline/ComputePipelineState.mpp>
#include <stormkit/render/pipeline/DescriptorSetLayout.mpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<render::ComputePipelineShaderState>::operator()(
        [[maybe_unused]] const render::ComputePipelineShaderState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader);

        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<render::ComputePipelineLayout>::operator()(
        const render::ComputePipelineLayout &state) const noexcept {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto &layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::Hash64 hash<render::ComputePipelineState>::operator()(
        const render::ComputePipelineState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader_state);
        core::hashCombine(hash, state.layout);

        return hash;
    }
} // namespace std
