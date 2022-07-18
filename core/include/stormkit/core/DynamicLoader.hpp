// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <array>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/Pimpl.hpp>

namespace stormkit::core {
    namespace details {
        struct DynamicLoaderImpl;
    }
    /**
     * @class SharedLibrary
     * @ingroup Core
     * @brief Shared Library representation class
     *
     * The class allow a simple access cross-os to a shared library
     */
    class STORMKIT_PUBLIC DynamicLoader {
      public:
        /**
         * @brief Constructor
         *
         * Constructor of SharedLibrary
         *
         * @param filename : the filename of the shared library (string)
         * assert(the filename can't be empty)
         */
        explicit DynamicLoader(std::filesystem::path filename);

        /**
         * @brief Destructor
         *
         * Destructor of SharedLibrary
         */
        ~DynamicLoader();

        DynamicLoader(const DynamicLoader &) = delete;
        auto operator=(const DynamicLoader &) -> DynamicLoader & = delete;

        DynamicLoader(DynamicLoader &&) noexcept;
        auto operator=(DynamicLoader &&) noexcept -> DynamicLoader &;

        [[nodiscard]] auto loaded() const noexcept -> bool;

        /**
         * @brief Get a shared symbol
         *
         * Method wich load and return the shared symbol of the shared library
         * currently loaded
         *
         * @param name : the name of the shared symbol (string) assert(the name
         * can't be empty)
         */
        template<class Signature>
        [[nodiscard]] auto func(std::string_view name) const -> std::function<Signature>;
        template<class Signature>
        [[nodiscard]] auto cFunc(std::string_view name) const -> Signature;

        [[nodiscard]] auto filepath() const noexcept -> const std::filesystem::path &;

      private:
        auto doGetFunc(std::string_view name) const -> void *;

        core::Pimpl<details::DynamicLoaderImpl> m_impl;

        bool m_is_loaded = false;

        std::filesystem::path m_filepath;
    };
    DECLARE_PTR_AND_REF(DynamicLoader)
} // namespace stormkit::core

#include "DynamicLoader.inl"
