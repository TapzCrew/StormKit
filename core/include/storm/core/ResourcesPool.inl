// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    template<typename... Args>
    _Value &ResourcesPool<_Key, _Value>::create(_Key &&key, Args &&...args) {
        auto it = m_resources.emplace(std::move(key), _Value { std::forward<Args>(args)... });
        return it.first->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    template<typename... Args>
    _Value &ResourcesPool<_Key, _Value>::create(const _Key &key, Args &&...args) {
        auto it = m_resources.emplace(key, _Value { std::forward<Args>(args)... });
        return it.first->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    _Value &ResourcesPool<_Key, _Value>::get(const _Key &key) {
        auto it = getIt(key);
        STORMKIT_EXPECTS(it != std::cend(m_resources));

        return it->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    const _Value &ResourcesPool<_Key, _Value>::get(const _Key &key) const {
        const auto it = getIt(key);
        STORMKIT_EXPECTS(it != std::cend(m_resources));

        return it->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    void ResourcesPool<_Key, _Value>::remove(const _Key &key) {
        const auto it = getIt(key);
        STORMKIT_EXPECTS(it != std::cend(m_resources));

        m_resources.erase(it);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    bool ResourcesPool<_Key, _Value>::has(const _Key &key) const noexcept {
        return getIt(key) != std::cend(m_resources);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    void ResourcesPool<_Key, _Value>::clear() {
        m_resources.clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    auto ResourcesPool<_Key, _Value>::getIt(const _Key &key) noexcept {
        return m_resources.find(key);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename _Key, typename _Value>
    auto ResourcesPool<_Key, _Value>::getIt(const _Key &key) const noexcept {
        return m_resources.find(key);
    }
} // namespace storm::core
