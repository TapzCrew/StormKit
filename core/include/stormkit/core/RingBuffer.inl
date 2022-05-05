// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "RingBuffer.mpp"

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    RingBuffer<T>::RingBuffer(SizeType capacity) : m_capacity { capacity } {
        m_buffer.resize(m_capacity * sizeof(ValueType));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    RingBuffer<T>::RingBuffer(const RingBuffer &copy) {
        m_capacity = copy.m_capacity;
        m_count    = copy.m_count;
        m_write    = copy.m_write;
        m_read     = copy.m_read;

        m_buffer.resize(m_capacity * sizeof(ValueType));
        if (!empty())
            for (auto i = m_read; i < m_write;) {
                new (&m_buffer[i * sizeof(ValueType)]) T { *copy.getPtr(i) };

                i += 1;
                if (i >= m_capacity) i -= m_capacity;
            }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::operator=(const RingBuffer &copy) -> RingBuffer & {
        if (&copy == this) return *this;

        m_capacity = copy.m_capacity;
        m_count    = copy.m_count;
        m_write    = copy.m_write;
        m_read     = copy.m_read;

        m_buffer.resize(m_capacity * sizeof(ValueType));
        if (!empty())
            for (auto i = m_read; i < m_write;) {
                new (&m_buffer[i * sizeof(ValueType)]) T { *copy.getPtr(i) };

                i += 1;
                if (i >= m_capacity) i -= m_capacity;
            }

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    RingBuffer<T>::RingBuffer(RingBuffer &&moved) noexcept {
        m_buffer = std::exchange(moved.m_buffer, ByteArray {});

        m_capacity = std::exchange(moved.m_capacity, 0);
        m_count    = std::exchange(moved.m_count, 0);
        m_write    = std::exchange(moved.m_write, 0);
        m_read     = std::exchange(moved.m_read, 0);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::operator=(RingBuffer &&moved) noexcept -> RingBuffer & {
        if (&moved == this) return *this;

        m_buffer = std::exchange(moved.m_buffer, ByteArray {});

        m_capacity = std::exchange(moved.m_capacity, 0);
        m_count    = std::exchange(moved.m_count, 0);
        m_write    = std::exchange(moved.m_write, 0);
        m_read     = std::exchange(moved.m_read, 0);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    RingBuffer<T>::~RingBuffer() {
        clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::clear() noexcept -> void {
        while (!empty()) pop();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::empty() const noexcept -> bool {
        return m_count == 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::full() const noexcept -> bool {
        return m_count == m_capacity;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::size() const noexcept -> SizeType {
        return m_count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::capacity() const noexcept -> SizeType {
        return m_capacity;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::push(ValueType value) -> bool {
        emplace(std::move(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    template<class... Args>
    auto RingBuffer<T>::emplace(Args &&...values) noexcept(
        std::is_nothrow_constructible_v<ValueType, Args &&...>) -> void {
        if (m_count == m_capacity) pop();

        new (&m_buffer[m_write * sizeof(ValueType)]) ValueType { std::forward<Args>(values)... };

        m_write += 1;
        if (m_write >= m_capacity) m_write -= m_capacity;

        m_count++;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::next() noexcept -> void {
        m_read += 1;
        if (m_read >= m_capacity) m_read -= m_capacity;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::pop() noexcept -> void {
        STORMKIT_EXPECTS(!empty());

        getPtr(m_write)->~ValueType();

        --m_count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::get() noexcept -> ValueType & {
        STORMKIT_EXPECTS(!empty());

        return *getPtr(m_read);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::get() const noexcept -> const ValueType & {
        STORMKIT_EXPECTS(!empty());

        return *getPtr(m_read);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::data() const noexcept -> std::span<const ValueType> {
        return std::span<const ValueType> { getPtr(0), m_capacity };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::getPtr(SizeType pos) noexcept -> ValueType * {
        return std::launder(reinterpret_cast<T *>(&m_buffer[pos * sizeof(ValueType)]));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto RingBuffer<T>::getPtr(SizeType pos) const noexcept -> const ValueType * {
        return std::launder(reinterpret_cast<const T *>(&m_buffer[pos * sizeof(ValueType)]));
    }
} // namespace stormkit::core
