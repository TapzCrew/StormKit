// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Vertex.hpp"

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    VertexArray::VertexArray(std::initializer_list<VertexType> datas)
        : m_data { sizeof(VertexType) * std::size(datas) } {
        const auto span = core::toConstByteSpan(datas);
        std::copy(std::begin(span), std::end(span), std::begin(m_data));
        // core::ranges::copy(span, m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::iteratorProxy() noexcept -> core::span<VertexType> {
        return { reinterpret_cast<VertexType *>(std::data(m_data)), count<VertexType>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::iteratorProxy() const noexcept -> core::span<const VertexType> {
        return { reinterpret_cast<const VertexType *>(std::data(m_data)), count<VertexType>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::begin() noexcept -> VertexData::iterator {
        return core::ranges::begin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::begin() const noexcept -> VertexData::const_iterator {
        return std::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::cbegin() const noexcept -> VertexData::const_iterator {
        return std::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::end() noexcept -> VertexData::iterator {
        return core::ranges::end(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::end() const noexcept -> VertexData::const_iterator {
        return std::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::cend() const noexcept -> VertexData::const_iterator {
        return std::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::clear() -> void { m_data.clear(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::resize(core::ArraySize size) -> void { m_data.resize(size); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::reserve(core::ArraySize size) -> void { m_data.reserve(size); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::push_back(core::Byte v) -> void { m_data.push_back(v); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::push_back(VertexType &&v) -> void {
        static constexpr auto VERTEX_SIZE = sizeof(VertexType);

        auto current_size = std::size(m_data);
        m_data.resize(current_size + VERTEX_SIZE);

        auto data_ptr = reinterpret_cast<const core::Byte *>(&v);
        std::copy(data_ptr, data_ptr + VERTEX_SIZE, std::data(m_data) + current_size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::pop_back() -> void { m_data.pop_back(); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::pop_back() -> void {
        static constexpr auto VERTEX_SIZE = sizeof(VertexType);

        m_data.resize(std::size(m_data) - VERTEX_SIZE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::operator[](core::ArraySize index) noexcept -> core::Byte & {
        STORMKIT_EXPECTS(index < size());
        return m_data[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::operator[](core::ArraySize index) const noexcept
        -> const core::Byte & {
        STORMKIT_EXPECTS(index < size());
        return m_data[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::at(core::ArraySize index) -> VertexType & {
        const auto size = count<VertexType>();
        STORMKIT_EXPECTS(index < size);

        return iteratorProxy<VertexType>()[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::at(core::ArraySize index) const -> const VertexType & {
        const auto size = count<VertexType>();
        STORMKIT_EXPECTS(index < size);

        return iteratorProxy<VertexType>()[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
    auto VertexArray::count() const noexcept -> core::ArraySize {
        static constexpr auto VERTEX_SIZE = sizeof(VertexType);
        return size() / VERTEX_SIZE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::size() const noexcept -> core::ArraySize { return std::size(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::empty() const noexcept -> bool { return std::empty(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::data() noexcept -> core::span<core::Byte> { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::data() const noexcept -> core::span<const core::Byte> {
        return m_data;
    }
} // namespace storm::engine
