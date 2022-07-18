// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/Strings.hpp>

#include <stormkit/log/Logger.hpp>

namespace stormkit::log {
    class STORMKIT_PUBLIC FileLogger final: public Logger {
      public:
        FileLogger(LogClock::time_point start, std::filesystem::path path);
        FileLogger(LogClock::time_point start, std::filesystem::path path, Severity log_level);
        ~FileLogger();

        FileLogger(const FileLogger &) = delete;
        auto operator=(const FileLogger &) -> FileLogger & = delete;

        FileLogger(FileLogger &&);
        auto operator=(FileLogger &&) -> FileLogger &;

        auto write(Severity severity, const Module &module, const char *string) -> void override;
        auto flush() -> void override;

      private:
        core::StringHashMap<std::ofstream> m_streams;

        std::filesystem::path m_base_path;
    };
} // namespace stormkit::log
