// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::core {
    class App;
    DECLARE_PTR_AND_REF(App)

    class NonCopyable;
    DECLARE_PTR_AND_REF(NonCopyable)

    class NonDefaultInstanciable;
    DECLARE_PTR_AND_REF(NonDefaultInstanciable)

    class NonMovable;
    DECLARE_PTR_AND_REF(NonMovable)

    class State;
    DECLARE_PTR_AND_REF(State)

    class StateManager;
    DECLARE_PTR_AND_REF(StateManager)
} // namespace storm::core

#if defined(__clang__) || defined(__GNUC__)
/// \brief FUNCTION is a macro which get the C++ symbol of the current function as
/// string.
/// \notes The symbol string is compiler dependant, do not make asumpsion of the output
    #define FUNCTION __PRETTY_FUNCTION__
#else
/// [unique-name](FUNCTION)
    #define FUNCTION __FUNCSIG__
#endif
