// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/log/Fwd.hpp>
#include <storm/log/Logger.hpp>

#include <string>
#include <string_view>

namespace storm::log {
    class STORMKIT_PUBLIC LogHandler final: public core::NonDefaultInstanciable {
      public:
        template<class T, typename... Args>
        static void setupLogger(Args &&...param_args);

        static void setupDefaultLogger();

        template<typename... Args>
        static void
            log(Severity severity, Module module, std::string_view format_string, Args &&...param_args);

        template<typename... Args>
        static inline void log(Severity severity, std::string_view format_string, Args &&...param_args);

        template<typename... Args>
        static inline void dlog(Args &&...param_args);

        template<typename... Args>
        static inline void ilog(Args &&...param_args);

        template<typename... Args>
        static inline void wlog(Args &&...param_args);

        template<typename... Args>
        static inline void elog(Args &&...param_args);

        template<typename... Args>
        static inline void flog(Args &&...param_args);

        static Logger &logger();

      private:
        static void setupLogger(LoggerOwnedPtr &&ptr);

        static inline LoggerOwnedPtr m_logger = nullptr;
    };
} // namespace storm::log

#define LOGGER(module)                                                               \
    static constexpr inline auto LOG_MODULE = storm::log::makeModule(module);        \
    template<typename... Args>                                                       \
    static inline void dlog(Args &&...param_args) {                                  \
        storm::log::LogHandler::dlog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                \
    template<typename... Args>                                                       \
    static inline void ilog(Args &&...param_args) {                                  \
        storm::log::LogHandler::ilog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                \
    template<typename... Args>                                                       \
    static inline void wlog(Args &&...param_args) {                                  \
        storm::log::LogHandler::wlog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                \
    template<typename... Args>                                                       \
    static inline void elog(Args &&...param_args) {                                  \
        storm::log::LogHandler::elog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }                                                                                \
    template<typename... Args>                                                       \
    static inline void flog(Args &&...param_args) {                                  \
        storm::log::LogHandler::flog(LOG_MODULE, std::forward<Args>(param_args)...); \
    }

#include "LogHandler.inl"
