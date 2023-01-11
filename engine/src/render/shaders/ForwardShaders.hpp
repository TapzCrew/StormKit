// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



#include <stormkit/Core/Types.hpp>

namespace stormkit::engine {
    static constexpr auto SHADER_DATA = core::makeStaticByteArray(
#include <FowardPass.spv.h>
    );
} // namespace stormkit::engine
