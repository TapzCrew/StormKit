// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::render {
    class Shader;
    DECLARE_PTR_AND_REF(Shader);

    class Texture;
    DECLARE_PTR_AND_REF(Texture);

    class TextureView;
    DECLARE_PTR_AND_REF(TextureView);

    class Sampler;
    DECLARE_PTR_AND_REF(Sampler);

    class HardwareBuffer;
    DECLARE_PTR_AND_REF(HardwareBuffer);

    struct TextureSubresource;
    struct TextureSubresourceLayers;
    struct TextureSubresourceRange;
} // namespace storm::render
