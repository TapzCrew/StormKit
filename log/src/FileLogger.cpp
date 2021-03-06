// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.filelogger;

// clang-format off
/////////// - STL - ///////////
import <string_view>;

/////////// - StormKit::core - ///////////
import stormkit.core.format
import stormkit.core.strings

/////////// - StormKit::log - ///////////
import stormkit.log.details.logcolorizer;
// clang-format on
#else
    //////////// - StormKit::core - ///////////
    #include <stormkit/core/Format.mpp>
    #include <stormkit/core/Strings.mpp>

/////////// - StormKit::log - ///////////
    #include <stormkit/log/FileLogger.mpp>
#endif

using namespace std::literals;

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    FileLogger::FileLogger(LogClock::time_point start, std::filesystem::path path)
        : Logger { std::move(start) }, m_base_path { std::move(path) } {
        if (!std::filesystem::exists(m_base_path)) std::filesystem::create_directory(m_base_path);

        STORMKIT_EXPECTS_MESSAGE(std::filesystem::is_directory(m_base_path),
                                 "path need to be a directory");

        auto filepath                = m_base_path / "log.txt";
        m_streams[filepath.string()] = std::ofstream { filepath.string() };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    FileLogger::FileLogger(LogClock::time_point start,
                           std::filesystem::path path,
                           Severity log_level)
        : Logger { std::move(start), log_level }, m_base_path { std::move(path) } {
        if (!std::filesystem::exists(m_base_path)) std::filesystem::create_directory(m_base_path);

        STORMKIT_EXPECTS_MESSAGE(std::filesystem::is_directory(m_base_path),
                                 "path need to be a directory");

        auto filepath                = m_base_path / "log.txt";
        m_streams[filepath.string()] = std::ofstream { filepath.string() };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    FileLogger::~FileLogger() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    FileLogger::FileLogger(FileLogger &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto FileLogger::operator=(FileLogger &&) -> FileLogger & = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto FileLogger::flush() -> void {
        for (auto &[path, stream] : m_streams) stream.flush();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto FileLogger::write(Severity severity, Module m, const char *string) -> void {
        const auto now = LogClock::now();
        const auto time =
            std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time).count();

        auto filepath = m_base_path / "log.txt";
        if (std::size(*m) == 0) {
            filepath = m_base_path / (*m + std::string { "-log.txt" });

            if (m_streams.find(filepath.string()) == m_streams.cend())
                m_streams[filepath.string()] = std::ofstream { filepath.string() };
        }

        static constexpr auto LOG_LINE        = "[{0}, {1}s] {2}\n"sv;
        static constexpr auto LOG_LINE_MODULE = "[{0}, {1}s, {2}] {3}\n"sv;

        auto final_string = std::string {};
        if (std::size(*m) == 0) final_string = core::format(LOG_LINE, severity, time, string);
        else
            final_string = core::format(LOG_LINE_MODULE, severity, time, *m, string);

        m_streams[filepath.string()] << final_string << std::flush;
    }
} // namespace stormkit::log
