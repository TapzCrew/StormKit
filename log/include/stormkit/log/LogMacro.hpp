// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#define LOGGER(module)                                                                  \
    static const inline auto LOG_MODULE = stormkit::log::makeModule(module);            \
    template<typename... Args>                                                          \
    inline auto dlog(Args &&...param_args)->void {                                      \
        stormkit::log::LogHandler::dlog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                   \
    template<typename... Args>                                                          \
    inline auto ilog(Args &&...param_args)->void {                                      \
        stormkit::log::LogHandler::ilog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                   \
    template<typename... Args>                                                          \
    inline auto wlog(Args &&...param_args)->void {                                      \
        stormkit::log::LogHandler::wlog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                   \
    template<typename... Args>                                                          \
    inline auto elog(Args &&...param_args)->void {                                      \
        stormkit::log::LogHandler::elog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                   \
    template<typename... Args>                                                          \
    inline auto flog(Args &&...param_args)->void {                                      \
        stormkit::log::LogHandler::flog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }
