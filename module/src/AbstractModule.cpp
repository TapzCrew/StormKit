// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/module/AbstractModule.hpp>

using namespace storm;
using namespace storm::module;

/////////////////////////////////////
/////////////////////////////////////
AbstractModule::AbstractModule(std::filesystem::path filepath)
    : m_filepath { std::move(filepath) } {
}

/////////////////////////////////////
/////////////////////////////////////
AbstractModule::~AbstractModule() = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractModule::AbstractModule(AbstractModule &&) = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractModule &AbstractModule::operator=(AbstractModule &&) = default;
