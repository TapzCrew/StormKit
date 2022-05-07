// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/NamedType.hpp>

#ifndef STORMKIT_IMAGE_NO_LOGGING
    /////////// - StormKit::log - ///////////
    #include <storm/log/LogHandler.hpp>
#endif

namespace storm::image {
#ifndef STORMKIT_IMAGE_NO_LOGGING
    template<typename... Args>
    inline void elog(Args &&...args) {
        static constexpr auto LOG_MODULE = log::makeModule("image");

        log::LogHandler::elog(LOG_MODULE, std::forward<Args>(args)...);
    }
#else
    template<typename... Args>
    inline void elog([[maybe_unused]] Args... &&args) {}
#endif

    class Image;
    DECLARE_PTR_AND_REF(Image)

    using XOffset = core::NamedType<core::UInt32, struct XOffsetTag>;
    using YOffset = core::NamedType<core::UInt32, struct YOffsetTag>;

    namespace private_ {
        struct ImageData;
        DECLARE_PTR_AND_REF(ImageData)
    } // namespace private_
} // namespace storm::image
