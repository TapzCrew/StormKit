#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>

#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/LogColorizer.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>

#include <iostream>

#include <climits>
#include <clocale>

using namespace std::literals;

using namespace storm;
using namespace storm::log;


////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level)
    : Logger { std::move(start), log_level } {
}

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::~ConsoleLogger() = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(ConsoleLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger &ConsoleLogger::operator=(ConsoleLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(const ConsoleLogger &) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger &ConsoleLogger::operator=(const ConsoleLogger &) = default;

////////////////////////////////////////
////////////////////////////////////////
void ConsoleLogger::flush() {
    std::cout.flush();
}

////////////////////////////////////////
////////////////////////////////////////
void ConsoleLogger::write(Severity severity, Module module, const char *string) {
    const auto now  = LogClock::now();
    const auto time = std::chrono::duration_cast<core::Secondf>(now - m_start_time);

    static constexpr auto LOG_LINE        = "[{0}, {1}s]"sv;
    static constexpr auto LOG_LINE_MODULE = "[{0}, {1}s, {2}]"sv;

    auto str = std::string {};

    if (std::char_traits<char>::length(module.get()) == 0)
        str = core::format(LOG_LINE, severity, time);
    else
        str = core::format(LOG_LINE_MODULE, severity, time, module.get());

    const auto to_stderr = severity == Severity::Error || severity == Severity::Fatal;
    auto &output         = (to_stderr) ? std::cerr : std::cout;

    // not yet
    /*
    auto state          = std::mbstate_t {};
    std::string out_str = std::string { MB_LEN_MAX };
    for (const auto &c : str) std::c8rtomb(std::data(out_str), c, &state);

    state                  = std::mbstate_t {};
    std::string out_string = std::string { MB_LEN_MAX };
    for (const auto &c : string) { std::c8rtomb(std::data(out_string), c, &state); }*/

    colorifyBegin(severity, to_stderr);
    output << str;
    colorifyEnd(to_stderr);
    output << " " << string << '\n' << std::flush;
}
