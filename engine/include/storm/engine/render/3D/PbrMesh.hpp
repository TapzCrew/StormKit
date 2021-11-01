// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Drawable.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC PbrMesh: public Drawable {
      public:
        struct alignas(16) Vertex {
            core::Vector3f position;
            core::Vector3f normal;
            core::Vector2f texcoord;
            core::Vector4f tangent;
            core::Vector4u joint_id = { 0.f, 0.f, 0.f, 0.f };
            core::Vector4f weight   = { 0.f, 0.f, 0.f, 0.f };
        };

        struct Node {
            using ID                      = core::UInt32;
            static constexpr auto INVALID = std::numeric_limits<ID>::max();
            static constexpr auto NO_SKIN = std::numeric_limits<core::UInt32>::max();

            ID parent = INVALID;
            std::vector<ID> childs;

            core::ArraySize sub_drawable_id = std::numeric_limits<core::ArraySize>::max();

            core::Vector3f translation = {};
            core::Vector3f scale       = core::Vector3f { 1.f };
            core::Quaternion rotation  = {};
            core::Matrix matrix        = core::Matrix { 1.f };
            core::UInt32 skin          = NO_SKIN;

            bool is_joint = false;

            core::Matrix localTransform() const noexcept;
            core::Matrix globalTransform(core::span<const Node> nodes) const noexcept;
        };

        struct Skin {
            std::string name;

            Node::ID root_joint = Node::INVALID;

            std::vector<core::Matrixf> inverse_bind_matrices;

            std::vector<Node::ID> joints;
        };

        enum class InterpolationType { Linear, Step, Cubic_Spline };

        struct AnimationSampler {
            InterpolationType interpolation;
            std::vector<float> inputs;
            std::vector<core::Vector4f> outputs;
        };

        enum class Path { Translation, Rotation, Scale, Weights };

        struct AnimationChannel {
            Path path;
            Node::ID node;
            core::UInt32 sampler;
        };

        struct Animation {
            std::string name;
            std::vector<AnimationSampler> samplers;
            std::vector<AnimationChannel> channels;

            core::Secondf start        = core::Secondf { std::numeric_limits<float>::max() };
            core::Secondf end          = core::Secondf { std::numeric_limits<float>::min() };
            core::Secondf current_time = core::Secondf { 0.f };
        };

        PbrMesh() noexcept;
        PbrMesh(std::string name);
        ~PbrMesh() override;

        PbrMesh(const PbrMesh &);
        PbrMesh &operator=(const PbrMesh &);

        PbrMesh(PbrMesh &&) noexcept;
        PbrMesh &operator=(PbrMesh &&) noexcept;

        void addAnimation(Animation animation);
        core::UInt32 addSkin(Skin skin);
        Node::ID addNode(Node node);
        const Node &getNode(Node::ID id) const noexcept;

        [[nodiscard]] constexpr core::ArraySize vertexSize() const noexcept override;

        ALLOCATE_HELPERS(PbrMesh)
      private:
        std::vector<Node> m_nodes;
        std::vector<Node::ID> m_root_nodes;

        std::vector<Skin> m_skins;
        std::vector<Animation> m_animations;
    };
} // namespace storm::engine

#include "PbrMesh.inl"
