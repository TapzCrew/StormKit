// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "ModuleImpl.hpp"

#include <storm/core/Strings.hpp>

#include <stdexcept>

#include <dlfcn.h>

using namespace storm;
using namespace storm::module;

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::ModuleImpl(std::filesystem::path filepath) : AbstractModule { std::move(filepath) } {
    m_library_handle = dlopen(m_filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);

    if (!m_library_handle) {
        auto system_reason = std::string(dlerror());
        throw std::runtime_error { fmt::format("Failed to load module \"{}\", reason: {}",
                                               m_filepath.string(),
                                               system_reason) };
    } else
        m_is_loaded = true;
}

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::~ModuleImpl() {
    if (m_library_handle) dlclose(m_library_handle);
}

/////////////////////////////////////
/////////////////////////////////////
ModuleImpl::ModuleImpl(ModuleImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
ModuleImpl &ModuleImpl::operator=(ModuleImpl &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void *ModuleImpl::_getFunc(std::string_view name) const {
    void *func = dlsym(m_library_handle, name.data());

    if (!func) {
        auto system_reason = std::string(dlerror());
        throw std::runtime_error { fmt::format("Failed to load function \"{}()\" (on module "
                                               "\"{}\"), reason: {}",
                                               name,
                                               m_filepath.string(),
                                               system_reason) };
    }

    return func;
}
