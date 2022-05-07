// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::module {
    /////////////////////////////////////
    /////////////////////////////////////
    bool Module::isLoaded() const noexcept { return m_impl->isLoaded(); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    std::function<Signature> Module::getFunc(std::string_view name) const {
        STORMKIT_EXPECTS(!name.empty());

        return m_impl->getFunc<Signature>(name);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    Signature Module::getCFunc(std::string_view name) const {
        STORMKIT_EXPECTS(!name.empty());

        return m_impl->getCFunc<Signature>(name);
    }
} // namespace storm::module
