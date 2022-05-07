#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<VertexInputAttributeDescription>::operator()(
        const VertexInputAttributeDescription &description) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.location);
        core::hashCombine(hash, description.format);
        core::hashCombine(hash, description.offset);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionArray>::operator()(
        const storm::render::VertexInputAttributeDescriptionArray &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionSpan>::operator()(
        const storm::render::VertexInputAttributeDescriptionSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }

    core::Hash64 hash<storm::render::VertexInputAttributeDescriptionConstSpan>::operator()(
        const storm::render::VertexInputAttributeDescriptionConstSpan &bindings) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &binding : bindings) core::hashCombine(hash, binding);

        return hash;
    }
} // namespace std
