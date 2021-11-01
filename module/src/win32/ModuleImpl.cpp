// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "ModuleImpl.hpp"

using namespace storm;
using namespace storm::module;

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::ModuleImpl(std::filesystem::path filepath)
    : AbstractModule { std::move(filepath) }, m_library_handle { nullptr } {
    m_library_handle =
        LoadLibraryW(m_filepath.c_str()); // TODO handle failing to load the shared object

    m_is_loaded = m_library_handle != nullptr;
}

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::~ModuleImpl() {
    if (m_library_handle) FreeLibrary(m_library_handle);
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
    return reinterpret_cast<void *>(GetProcAddress(m_library_handle, name.data()));
}
