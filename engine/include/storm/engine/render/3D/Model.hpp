// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>
#include <storm/engine/render/3D/PbrMesh.hpp>

namespace tinygltf {
    struct Mesh;
    class Node;
    struct Scene;
    class Model;
    struct Material;
    struct Primitive;
    struct Skin;
    struct Animation;
} // namespace tinygltf

namespace storm::engine {
    class STORMKIT_PUBLIC Model: public core::NonCopyable {
      public:
        explicit Model(Engine &engine) noexcept;
        explicit Model(Engine &engine, std::filesystem::path path);
        ~Model();

        Model(Model &&) noexcept;
        Model &operator=(Model &&) noexcept;

        void load(std::filesystem::path path);

        [[nodiscard]] PbrMeshOwnedPtr createMesh() const noexcept;
        [[nodiscard]] PbrMaterialOwnedPtr createMaterial() const noexcept;

        [[nodiscard]] bool hasMaterial() const noexcept;
        [[nodiscard]] bool loaded() const noexcept;
        [[nodiscard]] const std::filesystem::path &path() const noexcept;

        ALLOCATE_HELPERS(storm::engine::Model)
      private:
        SubDrawable doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh);
        DrawablePrimitive doParsePrimitive(const tinygltf::Model &model,
                                           const tinygltf::Primitive &primitive);
        PbrMesh::Skin doParseSkin(const tinygltf::Model &model, const tinygltf::Skin &skin);
        PbrMesh::Animation doParseAnimation(const tinygltf::Model &model,
                                            const tinygltf::Animation &animation);

        EngineRef m_engine;

        std::filesystem::path m_path;

        PbrMesh m_mesh;

        bool m_has_material = false;

        bool m_loaded = false;
    };
} // namespace storm::engine

#include "Model.inl"
