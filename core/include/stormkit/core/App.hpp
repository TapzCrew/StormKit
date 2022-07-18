// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>

#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/Types.hpp>

/// \module core
namespace stormkit::core {
    /// \group App
    /// \brief stormkit::core::App is an abstract class that encapsulate all the
    /// app logic.
    ///
    /// This class is a facility to keep the code aside from the main and split
    /// it in OO way.
    /// \unique_name App
    class App {
      public:
        /// \output_section Public Special Member Functions
        /// \brief Default constructor.
        App() noexcept = default;

        /// \brief Destructor.
        virtual ~App() noexcept = default;

        App(App &&) noexcept = delete;

        auto operator=(App &&) noexcept -> App & = delete;

        App(const App &) noexcept = delete;

        auto operator=(const App &) noexcept -> App & = delete;

        /// \output_section Public Member Functions
        /// \brief Run the app logic.
        ///
        /// This pure virtual function has to be implemented by the
        /// derived class to define the app logic.
        ///
        /// \param argc the program argument count.
        /// \param argv the program argument values.
        virtual auto run(const int argc, const char **argv) -> core::Int32 = 0;
    };
    DECLARE_PTR_AND_REF(App)
} // namespace stormkit::core
