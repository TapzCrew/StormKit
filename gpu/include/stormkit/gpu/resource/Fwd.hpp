// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>


#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::gpu {
    class Shader;
    DECLARE_PTR_AND_REF(Shader);

    class Image;
    DECLARE_PTR_AND_REF(Image);

    class ImageView;
    DECLARE_PTR_AND_REF(ImageView);

    class Sampler;
    DECLARE_PTR_AND_REF(Sampler);

    class Buffer;
    DECLARE_PTR_AND_REF(Buffer);

    struct ImageSubresource;
    struct ImageSubresourceLayers;
    struct ImageSubresourceRange;

    template<typename T>
    concept ResourceType = core::Is<T, Image> || core::Is<T, Buffer>;
} // namespace stormkit::gpu
