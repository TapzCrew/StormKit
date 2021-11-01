#include <storm/render/pipeline/VertexBindingDescription.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<VertexBindingDescription>::operator()(
        const VertexBindingDescription &description) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.stride);
        core::hashCombine(hash, description.input_rate);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexBindingDescriptionArray>::operator()(
        const storm::render::VertexBindingDescriptionArray &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexBindingDescriptionSpan>::operator()(
        const storm::render::VertexBindingDescriptionSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexBindingDescriptionConstSpan>::operator()(
        const storm::render::VertexBindingDescriptionConstSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }
} // namespace std
