// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <exception>
#include <iostream>

/////////// - GSL - ///////////
#include <gsl/gsl_assert>

/////////// - StormKit::core - ///////////
#include <storm/core/Fwd.hpp>
#include <storm/core/Platform.hpp>

#ifdef ASSERT
    #undef ASSERT
#endif

#if defined(STORMKIT_BUILD_DEBUG) || defined(STORMKIT_GEN_DOC)
    /// \brief `ASSERT` define an assertion, a confition that should be satisfied where it appears
    /// in a function body.
    ///
    /// If the invariant is not satisfied, the program will stop, printing the
    /// message in the standard error output.
    ///
    /// \param condition The condition of the assertion.
    /// \param message The failure message.
    /// \notes Disabled on release build.
    #define ASSERT(condition, message)                                                       \
        do {                                                                                 \
            if (GSL_LIKELY(!(condition))) {                                                  \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " \
                          << __LINE__ << "\n " << FUNCTION << ": " << message << std::endl;  \
                std::terminate();                                                            \
            }                                                                                \
        } while (false)
#else
    #define ASSERT(condition, message)
#endif

#if defined(STORMKIT_BUILD_DEBUG) || defined(STORMKIT_GEN_DOC)
    /// \brief `STORMKIT_ENSURES` define a postcondition, a condition that a function should ensure
    /// before returning value the state of other object upon entry the function.
    ///
    /// If the invariant is not satisfied, the program will stop, printing the
    /// message in the standard error output.
    ///
    /// \param condition The precondition.
    /// \notes Disabled on release build.
    #define STORMKIT_ENSURES(condition)                                                            \
        do {                                                                                       \
            if (GSL_LIKELY(!(condition))) {                                                        \
                std::cerr << "A postcondition `" #condition "` failed in " << __FILE__ << " line " \
                          << __LINE__ << "\n " << FUNCTION << std::endl;                           \
            }                                                                                      \
            Ensures(condition);                                                                    \
        } while (false)

    /// \brief Same as `STORMKIT_ENSURES` but with a custom failure message.
    /// \see
    /// \param condition The precondition.
    /// \param message The failure message.
    #define STORMKIT_ENSURES_MESSAGE(condition, message)                                           \
        do {                                                                                       \
            if (GSL_LIKELY(!(condition))) {                                                        \
                std::cerr << "A postcondition `" #condition "` failed in " << __FILE__ << " line " \
                          << __LINE__ << "\n " << FUNCTION << std::endl;                           \
                std::cerr << message << std::endl;                                                 \
            }                                                                                      \
            Ensures(condition);                                                                    \
        } while (false)

    /// \brief `STORMKIT_EXPECTS` define a precondition, the function expectation of its arguments
    /// and/or the state of other object upon entry the function.
    ///
    /// If the invariant is not satisfied, the program will stop, printing the
    /// message in the standard error output.
    ///
    /// \param condition The precondition.
    /// \notes Disabled on release build.
    #define STORMKIT_EXPECTS(condition)                                                           \
        do {                                                                                      \
            if (GSL_LIKELY(!(condition))) {                                                       \
                std::cerr << "A precondition `" #condition "` failed in " << __FILE__ << " line " \
                          << __LINE__ << "\n " << FUNCTION << std::endl;                          \
            }                                                                                     \
            Expects(condition);                                                                   \
        } while (false)

    /// \brief Same as `STORMKIT_EXPECTS` but with a custom failure message.
    /// \param condition The precondition.
    /// \param message The failure message.
    #define STORMKIT_EXPECTS_MESSAGE(condition, message)                                          \
        do {                                                                                      \
            if (GSL_LIKELY(!(condition))) {                                                       \
                std::cerr << "A precondition `" #condition "` failed in " << __FILE__ << " line " \
                          << __LINE__ << "\n " << FUNCTION << std::endl;                          \
                std::cerr << message << std::endl;                                                \
            }                                                                                     \
            Expects(condition);                                                                   \
        } while (false)
#else
    #define STORMKIT_ENSURES(condition) Ensures(condition)
    #define STORMKIT_ENSURES_MESSAGE(condition, message)
    #define STORMKIT_EXPECTS(condition) Expects(condition)
    #define STORMKIT_EXPECTS_MESSAGE(condition, message)
#endif
