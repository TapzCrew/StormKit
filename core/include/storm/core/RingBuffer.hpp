// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <gsl/gsl_util>

#include <storm/core/Assert.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Span.hpp>

namespace storm::core {
    template<typename T>
    class RingBuffer {
      public:
        using value_type = T;
        using size_type  = core::ArraySize;

        RingBuffer(size_type capacity) : m_capacity { capacity } {
            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type> { getPtr(0), m_capacity };
        }

        RingBuffer(const RingBuffer &copy) {
            m_capacity = copy.m_capacity;
            m_count    = copy.m_count;
            m_write    = copy.m_write;
            m_read     = copy.m_read;

            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type> { getPtr(0), m_capacity };
            if (!empty())
                for (auto i = m_read; i < m_write;) {
                    new (&m_buffer[i * sizeof(value_type)]) T { *copy.getPtr(i) };

                    i += 1;
                    if (i >= m_capacity) i -= m_capacity;
                }
        }

        RingBuffer &operator=(const RingBuffer &copy) {
            if (&copy == this) return *this;

            m_capacity = copy.m_capacity;
            m_count    = copy.m_count;
            m_write    = copy.m_write;
            m_read     = copy.m_read;

            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type> { getPtr(0), m_capacity };
            if (!empty())
                for (auto i = m_read; i < m_write;) {
                    new (&m_buffer[i * sizeof(value_type)]) T { *copy.getPtr(i) };

                    i += 1;
                    if (i >= m_capacity) i -= m_capacity;
                }

            return *this;
        }

        RingBuffer(RingBuffer &&moved) noexcept {
            m_buffer = std::move(moved.m_buffer);

            m_capacity = moved.m_capacity;
            m_count    = moved.m_count;
            m_write    = moved.m_write;
            m_read     = moved.m_read;

            moved.m_capacity = 0;
            moved.m_write    = 0;
            moved.m_read     = 0;
        }

        RingBuffer &operator=(RingBuffer &&moved) noexcept {
            if (&moved == this) return *this;

            m_buffer = std::move(moved.m_buffer);

            m_capacity = moved.m_capacity;
            m_count    = moved.m_count;
            m_write    = moved.m_write;
            m_read     = moved.m_read;

            moved.m_capacity = 0;
            moved.m_count    = 0;
            moved.m_write    = 0;
            moved.m_read     = 0;

            return *this;
        }

        ~RingBuffer() { clear(); }

        inline void clear() noexcept {
            while (!empty()) pop();
        }
        inline bool empty() const noexcept { return m_count == 0; }
        inline bool full() const noexcept { return m_count == m_capacity; }
        inline size_type size() const noexcept { return m_count; }
        inline size_type capacity() const noexcept { return m_capacity; }

        inline void push(value_type value) { emplace(std::move(value)); }

        template<typename... Args>
        void emplace(Args &&...values) noexcept(
            std::is_nothrow_constructible<value_type, Args &&...>::value) {
            if (m_count == m_capacity) pop();

            new (&m_buffer[m_write * sizeof(value_type)])
                value_type { std::forward<Args>(values)... };

            m_write += 1;
            if (m_write >= m_capacity) m_write -= m_capacity;

            m_count++;
        }

        void next() noexcept {
            m_read += 1;
            if (m_read >= m_capacity) m_read -= m_capacity;
        }

        void pop() noexcept {
            STORMKIT_EXPECTS(!empty());

            getPtr(m_write)->~value_type();

            --m_count;
        }

        [[nodiscard]] inline value_type &get() noexcept {
            STORMKIT_EXPECTS(!empty());

            return *getPtr(m_read);
        }

        [[nodiscard]] inline const value_type &get() const noexcept {
            STORMKIT_EXPECTS(!empty());

            return *getPtr(m_read);
        }

        [[nodiscard]] inline core::span<const value_type> data() const noexcept { return m_view; }

      private:
        [[nodiscard]] inline value_type *getPtr(size_type pos) noexcept {
            return std::launder(reinterpret_cast<T *>(&m_buffer[pos * sizeof(value_type)]));
        }

        [[nodiscard]] inline const value_type *getPtr(size_type pos) const noexcept {
            return std::launder(reinterpret_cast<const T *>(&m_buffer[pos * sizeof(value_type)]));
        }

        size_type m_capacity;
        size_type m_count = 0;

        std::vector<core::Byte> m_buffer;
        core::span<const value_type> m_view;

        size_type m_write = 0;
        size_type m_read  = 0;
    };
} // namespace storm::core
