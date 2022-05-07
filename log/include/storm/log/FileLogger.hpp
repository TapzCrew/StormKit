// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <filesystem>
#include <fstream>

#include <storm/core/HashMap.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/log/Logger.hpp>

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

namespace storm::log {
    class STORMKIT_PUBLIC FileLogger final: public storm::core::NonCopyable, public Logger {
      public:
        explicit FileLogger(LogClock::time_point start,
                            std::filesystem::path path,
                            Severity log_level = Logger::DEFAULT_SEVERITY);
        ~FileLogger() override;

        FileLogger(FileLogger &&);
        FileLogger &operator=(FileLogger &&);

        void write(Severity severity, Module module, const char *string) override;
        void flush() override;

      private:
        storm::core::HashMap<std::string, std::ofstream> m_streams;

        std::filesystem::path m_base_path;
    };
} // namespace storm::log

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(pop)
#endif
