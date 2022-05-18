// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#define NAMED_LOGGER(NAME, module) static const inline auto NAME = stormkit::log::Module { module };

#define LOGGER(module)                                \
    NAMED_LOGGER(LOG_MODULE, module)                  \
    template<typename... Args>                        \
    auto dlog(Args &&...args)->void {                 \
        LOG_MODULE.dlog(std::forward<Args>(args)...); \
    }                                                 \
    template<typename... Args>                        \
    auto ilog(Args &&...args)->void {                 \
        LOG_MODULE.ilog(std::forward<Args>(args)...); \
    }                                                 \
    template<typename... Args>                        \
    auto wlog(Args &&...args)->void {                 \
        LOG_MODULE.wlog(std::forward<Args>(args)...); \
    }                                                 \
    template<typename... Args>                        \
    auto elog(Args &&...args)->void {                 \
        LOG_MODULE.elog(std::forward<Args>(args)...); \
    }                                                 \
    template<typename... Args>                        \
    auto flog(Args &&...args)->void {                 \
        LOG_MODULE.flog(std::forward<Args>(args)...); \
    }
