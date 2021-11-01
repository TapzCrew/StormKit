#include <iostream>

/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Strings.hpp>

#include <storm/log/FileLogger.hpp>

using namespace std::literals;
using namespace storm;
using namespace storm::log;

////////////////////////////////////////
////////////////////////////////////////
FileLogger::FileLogger(LogClock::time_point start, std::filesystem::path path, Severity log_level)
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
FileLogger &FileLogger::operator=(FileLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void FileLogger::flush() {
    for (auto &[path, stream] : m_streams) stream.flush();
}

////////////////////////////////////////
////////////////////////////////////////
void FileLogger::write(Severity severity, Module module, const char *string) {
    const auto now  = LogClock::now();
    const auto time = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time).count();

    auto filepath = m_base_path / "log.txt";
    if (std::char_traits<char>::length(module.get()) == 0) {
        filepath = m_base_path / (module.get() + std::string { "-log.txt" });

        if (m_streams.find(filepath.string()) == m_streams.cend())
            m_streams[filepath.string()] = std::ofstream { filepath.string() };
    }

    static constexpr auto LOG_LINE        = "[{0}, {1}s] {2}\n"sv;
    static constexpr auto LOG_LINE_MODULE = "[{0}, {1}s, {2}] {3}\n"sv;

    auto final_string = std::string {};
    if (std::char_traits<char>::length(module.get()) == 0)
        final_string = core::format(LOG_LINE, severity, time, string);
    else
        final_string = core::format(LOG_LINE_MODULE, severity, time, module.get(), string);

    m_streams[filepath.string()] << final_string << std::flush;
}
