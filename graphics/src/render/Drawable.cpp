/////////// - StormKit::engine - ///////////
#include <storm/engine/render/Drawable.hpp>

using namespace storm;
using namespace storm::engine;

namespace std {
    core::Hash64 hash<Drawable>::operator()(const Drawable &drawable) const noexcept {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, drawable.name());
        for (const auto &sub_drawable : drawable.subDrawables()) {
            core::hashCombine(hash, sub_drawable.name);

            for (const auto &primitive : sub_drawable.primitives) {
                core::hashCombine(hash, primitive.name);
                core::hashCombine(hash, primitive.vertices.data());
                core::hashCombine(hash, primitive.indices);
                core::hashCombine(hash, primitive.first_vertex);
                core::hashCombine(hash, primitive.first_index);
                core::hashCombine(hash, primitive.material_index);
            }
        }

        return hash;
    }
} // namespace std

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(std::string name) : m_name { std::move(name) } {
}

////////////////////////////////////////
////////////////////////////////////////
Drawable::~Drawable() = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(const Drawable &) = default;

////////////////////////////////////////
////////////////////////////////////////
auto Drawable::operator=(const Drawable &) -> Drawable & = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(Drawable &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Drawable::operator=(Drawable &&) noexcept -> Drawable & = default;

////////////////////////////////////////
////////////////////////////////////////
core::ArraySize Drawable::addSubdrawable(SubDrawable subdrawable) {
    auto id = std::size(m_sub_drawables);

    m_sub_drawables.emplace_back(std::move(subdrawable));

    m_dirty = true;

    return id;
}

////////////////////////////////////////
////////////////////////////////////////
void Drawable::recomputeBoundingBox() noexcept {
    m_bounding_box.min = { 0.f, 0.f, 0.f };
    m_bounding_box.max = { 0.f, 0.f, 0.f };

    for (auto &sub_drawable : m_sub_drawables) {
        sub_drawable.bounding_box.min = { 0.f, 0.f, 0.f };
        sub_drawable.bounding_box.max = { 0.f, 0.f, 0.f };

        for (auto &primitive : sub_drawable.primitives) {
            sub_drawable.bounding_box.min =
                core::min(sub_drawable.bounding_box.min, primitive.bounding_box.min);
            sub_drawable.bounding_box.max =
                core::max(sub_drawable.bounding_box.max, primitive.bounding_box.max);
        }

        const auto extent = sub_drawable.bounding_box.max - sub_drawable.bounding_box.min;

        sub_drawable.bounding_box.extent.width  = extent.x;
        sub_drawable.bounding_box.extent.height = extent.y;
        sub_drawable.bounding_box.extent.depth  = extent.z;
    }

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;

    m_dirty = false;
}
