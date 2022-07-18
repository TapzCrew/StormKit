// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/Memory.hpp>

namespace stormkit::engine {
    static constexpr auto SHADER_DATA = core::makeStaticByteArray(
#include <FowardPass.spv.h>
    );
} // namespace stormkit::engine
