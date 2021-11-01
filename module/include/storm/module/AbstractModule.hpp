// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>

#include <filesystem>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/module/Fwd.hpp>

namespace storm::module {
    class STORMKIT_PUBLIC AbstractModule: public core::NonCopyable {
      public:
        explicit AbstractModule(std::filesystem::path filepath);
        virtual ~AbstractModule();

        AbstractModule(AbstractModule &&);
        AbstractModule &operator=(AbstractModule &&);

        inline bool isLoaded() const noexcept;

        template<class Signature>
        inline std::function<Signature> getFunc(std::string_view name);
        template<class Signature>
        inline Signature getCFunc(std::string_view name);

        inline const std::filesystem::path &getFilepath() const;

      protected:
        virtual void *_getFunc(std::string_view name) const = 0;

        bool m_is_loaded = false;

        std::filesystem::path m_filepath;
    };
} // namespace storm::module

#include "AbstractModule.inl"
