// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <variant>
#include <vector>

#include <cstddef>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC VertexArray {
      public:
        using VertexData = std::vector<core::Byte>;

        VertexArray() noexcept;
        template<typename VertexType>
        VertexArray(std::initializer_list<VertexType> data);
        ~VertexArray();

        VertexArray(const VertexArray &);
        VertexArray &operator=(const VertexArray &);

        VertexArray(VertexArray &&) noexcept;
        VertexArray &operator=(VertexArray &&) noexcept;

        template<typename VertexType>
        core::span<VertexType> iteratorProxy() noexcept;
        template<typename VertexType>
        core::span<const VertexType> iteratorProxy() const noexcept;

        VertexData::iterator begin() noexcept;
        VertexData::const_iterator begin() const noexcept;
        VertexData::const_iterator cbegin() const noexcept;

        VertexData::iterator end() noexcept;
        VertexData::const_iterator end() const noexcept;
        VertexData::const_iterator cend() const noexcept;

        void clear();
        void resize(core::ArraySize size);
        void reserve(core::ArraySize size);

        void push_back(core::Byte value);
        template<typename VertexType>
        void push_back(VertexType &&value);
        void pop_back();
        template<typename VertexType>
        void pop_back();

        core::Byte &operator[](core::ArraySize index) noexcept;
        const core::Byte &operator[](core::ArraySize index) const noexcept;

        template<typename VertexType>
        VertexType &at(core::ArraySize index);
        template<typename VertexType>
        const VertexType &at(core::ArraySize index) const;

        template<typename VertexType>
        core::ArraySize count() const noexcept;
        core::ArraySize count(core::ArraySize size_of_vertex) const noexcept;

        core::ArraySize size() const noexcept;
        bool empty() const noexcept;

        core::span<core::Byte> data() noexcept;
        core::ByteConstSpan data() const noexcept;

      private:
        VertexData m_data;
    };
} // namespace storm::engine

#include "Vertex.inl"
