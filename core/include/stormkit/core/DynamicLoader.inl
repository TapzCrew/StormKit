// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "DynamicLoader.mpp"

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DynamicLoader::loaded() const noexcept -> bool { return m_is_loaded; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    auto DynamicLoader::func(std::string_view name) const -> std::function<Signature> {
        STORMKIT_EXPECTS(!name.empty());

        return std::function<Signature>(reinterpret_cast<Signature *>(doGetFunc(name)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    auto DynamicLoader::cFunc(std::string_view name) const -> Signature {
        STORMKIT_EXPECTS(!name.empty());

        return reinterpret_cast<Signature>(doGetFunc(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DynamicLoader::filepath() const noexcept -> const std::filesystem::path & {
        return m_filepath;
    }
} // namespace stormkit::core
