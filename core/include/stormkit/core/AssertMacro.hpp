// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <stormkit/core/PlatformMacro.hpp>

#include <cassert>
#include <cstdio>
#include <cstdlib>

#if defined(STORMKIT_BUILD_DEBUG) || defined(STORMKIT_GEN_DOC)
namespace stormkit::core::details {
    struct AssertFailure {
        AssertFailure(const char *type, const char *message) {
            std::fprintf(stderr, "%s failure: %s", type, message);
            std::quick_exit(EXIT_FAILURE);
        }
    };
} // namespace stormkit::core::details

    #define STORMKIT_ASSERT_LINE STORMKIT_STRINGIFY(STORMKIT_CURRENT_LINE)
    #define STORMKIT_ASSERT_BASE(condition, type, message)                                \
        do {                                                                              \
            if (!(condition)) [[unlikely]] {                                              \
                std::cerr << type " `" #condition "` failed in " << STORMKIT_CURRENT_FILE \
                          << " line " << STORMKIT_CURRENT_LINE << "\n "                   \
                          << STORMKIT_CURRENT_FUNCTION << "\n\t" << message << std::endl; \
                stormkit::core::printStacktrace();                                        \
                std::quick_exit(EXIT_FAILURE);                                            \
            }                                                                             \
        } while (false)

    #define STORMKIT_CONSTEXPR_ASSERT_BASE(condition, type, message)                        \
        if (std::is_constant_evaluated())                                                   \
            (!(condition)) ? throw stormkit::core::details::AssertFailure { type, message } \
                           : void(0);                                                       \
        else                                                                                \
            STORMKIT_ASSERT_BASE(condition, type, message)

/// \brief `STORMKIT_ASSERT` define an assertion, a confition that should be satisfied where it
/// appears in a function body.
///
/// If the invariant is not satisfied, the program will stop, printing the
/// message in the standard error output.
///
/// \param condition The condition of the assertion.
/// \param message The failure message.
/// \notes Disabled on release build.
    #define STORMKIT_ASSERT(condition) STORMKIT_ASSERT_BASE(condition, "Assertion", "")

    #define STORMKIT_ASSERT_MESSAGE(condition, message) \
        STORMKIT_ASSERT_BASE(condition, "Assertion", message)

/// \brief `STORMKIT_ENSURES` define a postcondition, a condition that a function should ensure
/// before returning value the state of other object upon entry the function.
///
/// If the invariant is not satisfied, the program will stop, printing the
/// message in the standard error output.
///
/// \param condition The precondition.
/// \notes Disabled on release build.
    #define STORMKIT_ENSURES(condition) STORMKIT_ASSERT_BASE(condition, "Postcondition", "")

    /// \brief Same as `STORMKIT_ENSURES` but with a custom failure message.
    /// \see
    /// \param condition The precondition.
    /// \param message The failure message.
    #define STORMKIT_ENSURES_MESSAGE(condition, message) \
        STORMKIT_ASSERT_BASE(condition, "Postcondition", message)

    /// \brief `STORMKIT_EXPECTS` define a precondition, the function expectation of its arguments
    /// and/or the state of other object upon entry the function.
    ///
    /// If the invariant is not satisfied, the program will stop, printing the
    /// message in the standard error output.
    ///
    /// \param condition The precondition.
    /// \notes Disabled on release build.
    #define STORMKIT_EXPECTS(condition) STORMKIT_ASSERT_BASE(condition, "Precondition", "")

    /// \brief Same as `STORMKIT_EXPECTS` but with a custom failure message.
    /// \param condition The precondition.
    /// \param message The failure message.
    #define STORMKIT_EXPECTS_MESSAGE(condition, message) \
        STORMKIT_ASSERT_BASE(condition, "Precondition", message)

    #define STORMKIT_CONSTEXPR_ASSERT(condition) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Assertion", "")
    #define STORMKIT_CONSTEXPR_ASSERT_MESSAGE(condition, message) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Assertion", message)
    #define STORMKIT_CONSTEXPR_ENSURES(condition) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Precondition", "")
    #define STORMKIT_CONSTEXPR_ENSURES_MESSAGE(condition, message) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Precondition", message)
    #define STORMKIT_CONSTEXPR_EXPECTS(condition) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Postcondition", "")
    #define STORMKIT_CONSTEXPR_EXPECTS_MESSAGE(condition, message) \
        STORMKIT_CONSTEXPR_ASSERT_BASE(condition, "Postcondition", message)
#else
    #define STORMKIT_ENSURES(condition)
    #define STORMKIT_ENSURES_MESSAGE(condition, message)
    #define STORMKIT_EXPECTS(condition)
    #define STORMKIT_EXPECTS_MESSAGE(condition, message)
    #define STORMKIT_ASSERT(condition)
    #define STORMKIT_ASSERT_MESSAGE(condition, message)
    #define STORMKIT_CONSTEXPR_ENSURES(condition)
    #define STORMKIT_CONSTEXPR_ENSURES_MESSAGE(condition, message)
    #define STORMKIT_CONSTEXPR_EXPECTS(condition)
    #define STORMKIT_CONSTEXPR_EXPECTS_MESSAGE(condition, message)
    #define STORMKIT_CONSTEXPR_ASSERT(condition)
    #define STORMKIT_CONSTEXPR_ASSERT_MESSAGE(condition, message)
#endif
