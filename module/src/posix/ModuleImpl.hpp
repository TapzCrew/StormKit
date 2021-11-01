// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/module/AbstractModule.hpp>

namespace storm::module {
    class ModuleImpl final: public AbstractModule {
      public:
        explicit ModuleImpl(std::filesystem::path filename);
        ~ModuleImpl() override;

        ModuleImpl(ModuleImpl &&);
        ModuleImpl &operator=(ModuleImpl &&);

      protected:
        void *_getFunc(std::string_view name) const override;

      private:
        void *m_library_handle = nullptr;
    };
} // namespace storm::module
