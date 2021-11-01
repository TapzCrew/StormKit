// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <string>
#include <string_view>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/BoundingBox.hpp>
#include <storm/engine/render/Vertex.hpp>

namespace storm::engine {
    struct DrawablePrimitive {
        std::string name = "";

        VertexArray vertices;
        LargeIndexArray indices;

        core::UInt32 first_vertex = 0u;
        core::UInt32 first_index  = 0u;

        core::UInt32 material_index = 0u;

        BoundingBox bounding_box;
    };

    struct SubDrawable {
        std::string name = "";

        std::vector<DrawablePrimitive> primitives;

        BoundingBox bounding_box;
    };

    class STORMKIT_PUBLIC Drawable {
      public:
        Drawable() noexcept;
        explicit Drawable(std::string name);
        virtual ~Drawable();

        Drawable(const Drawable &);
        Drawable &operator=(const Drawable &);

        Drawable(Drawable &&) noexcept;
        Drawable &operator=(Drawable &&) noexcept;

        [[nodiscard]] std::string_view name() const noexcept;

        core::ArraySize addSubdrawable(SubDrawable subdrawable);
        [[nodiscard]] core::span<const SubDrawable> subDrawables() const noexcept;

        void recomputeBoundingBox() noexcept;
        [[nodiscard]] const BoundingBox &boundingBox() const noexcept;

        [[nodiscard]] virtual constexpr core::ArraySize vertexSize() const noexcept = 0;

        [[nodiscard]] bool dirty() const noexcept;

      protected:
        std::string m_name;

        std::vector<SubDrawable> m_sub_drawables;

        BoundingBox m_bounding_box;

        bool m_dirty = true;
    };
} // namespace storm::engine

HASH_FUNC(storm::engine::Drawable)

#include "Drawable.inl"
