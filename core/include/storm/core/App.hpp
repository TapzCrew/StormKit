// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>

/// \module core
namespace storm::core {
    /// \group App
    /// \brief storm::core::App is an abstract class that encapsulate all the
    /// app logic.
    ///
    /// This class is a facility to keep the code aside from the main and split
    /// it in OO way.
    /// \unique_name App
    class App: public storm::core::NonCopyable, public storm::core::NonMovable {
      public:
        /// \output_section Public Special Member Functions
        /// \brief Default constructor.
        App() noexcept = default;

        /// \brief Destructor.
        virtual ~App() noexcept = default;

        /// \output_section Public Member Functions
        /// \brief Run the app logic.
        ///
        /// This pure virtual function has to be implemented by the
        /// derived class to define the app logic.
        ///
        /// \param argc the program argument count.
        /// \param argv the program argument values.
        virtual void run(const int argc, const char **argv) = 0;
    };
} // namespace storm::core
