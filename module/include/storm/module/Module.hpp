// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <filesystem>
#include <functional>

#include <gsl/gsl_assert>

#include <storm/core/Assert.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/module/AbstractModule.hpp>

/**
 * @file SharedLibrary.hpp
 * @brief Shared Library tools
 * @author Arthapz
 * @version 0.1
 **/

namespace storm::module {
    /**
     * @class SharedLibrary
     * @ingroup Core
     * @brief Shared Library representation class
     *
     * The class allow a simple access cross-os to a shared library
     */
    class STORMKIT_PUBLIC Module: public core::NonCopyable {
      public:
        /**
         * @brief Constructor
         *
         * Constructor of SharedLibrary
         *
         * @param filename : the filename of the shared library (string)
         * assert(the filename can't be empty)
         */
        explicit Module(std::filesystem::path filename);

        /**
         * @brief Destructor
         *
         * Destructor of SharedLibrary
         */
        ~Module();

        Module(Module &&);
        Module &operator=(Module &&);

        inline bool isLoaded() const noexcept;

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
        inline std::function<Signature> getFunc(std::string_view name) const;
        template<class Signature>
        inline Signature getCFunc(std::string_view name) const;

      private:
        AbstractModuleOwnedPtr m_impl;
    };
} // namespace storm::module

#include "Module.inl"
