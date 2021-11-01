// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/module/Module.hpp>

#if defined(STORMKIT_OS_WINDOWS)
    #include "win32/ModuleImpl.hpp"
using Impl = storm::module::ModuleImpl;
#elif defined(STORMKIT_POSIX)
    #include "posix/ModuleImpl.hpp"
using Impl = storm::module::ModuleImpl;
#endif

using namespace storm;
using namespace storm::module;

/////////////////////////////////////
/////////////////////////////////////
Module::Module(std::filesystem::path filename) : m_impl { nullptr } {
    STORMKIT_EXPECTS(!filename.empty());

    m_impl = std::make_unique<Impl>(filename);
}

/////////////////////////////////////
/////////////////////////////////////
Module::~Module() = default;

/////////////////////////////////////
/////////////////////////////////////
Module::Module(Module &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Module &Module::operator=(Module &&) = default;
