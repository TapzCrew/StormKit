/////////// - StormKit::engine - ///////////
#include <storm/engine/Profiler.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Assert.hpp>
#include <storm/core/Ranges.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler::~Profiler() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler(const Profiler &) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Profiler::operator=(const Profiler &) noexcept -> Profiler & = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler(Profiler &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Profiler::operator=(Profiler &&) noexcept -> Profiler & = default;

/////////////////////////////////////
/////////////////////////////////////
auto Profiler::newFrame() -> void {
    if (GSL_LIKELY(!m_first_iteration)) {
        auto &previous = m_entries[m_current_entry];

        if (++m_current_entry >= ENTRY_COUNT) m_current_entry = 0;

        auto &current = m_entries[m_current_entry];
        current.scopes.clear();

        current.start = Clock::now();
        previous.end  = current.start;
    } else {
        auto &current = m_entries[m_current_entry];

        current.start = Clock::now();

        m_first_iteration = false;
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto Profiler::beginStage(std::string name, core::UInt32 level) -> void {
    auto &entry = m_entries[m_current_entry];

    auto &[_, scope] = entry.scopes.emplace_back(std::move(name), Scope {});

    scope.level     = level;
    scope.start     = Clock::now();
    scope.finalized = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto Profiler::endStage(std::string_view name) -> void {
    auto &entry = m_entries[m_current_entry];

    auto it = core::ranges::find_if(entry.scopes,
                                    [&name](const auto &pair) { return pair.first == name; });

    STORMKIT_EXPECTS(it != core::ranges::end(entry.scopes));

    auto &scope = it->second;

    STORMKIT_EXPECTS(scope.finalized == false);

    scope.end       = Clock::now();
    scope.finalized = true;
}
