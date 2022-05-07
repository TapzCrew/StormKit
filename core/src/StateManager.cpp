#if defined(STORMKIT_CXX20_MODULES)
module stormkit.core.statemanager;
#else
    /////////// - StormKit::core - ///////////
    #include <stormkit/core/State.mpp>
    #include <stormkit/core/StateManager.mpp>
#endif

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    StateManager::StateManager() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    StateManager::~StateManager() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    StateManager::StateManager(StateManager &&) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::operator=(StateManager &&) noexcept -> StateManager & = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::requestPush(StateOwnedPtr &&state) -> void {
        m_action_queue.emplace(
            StateManagerAction { StateManagerAction::Type::push, std::move(state) });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::requestSet(StateOwnedPtr &&state) -> void {
        m_action_queue.emplace(
            StateManagerAction { StateManagerAction::Type::set, std::move(state) });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::requestPop() -> void {
        if (!m_stack.empty())
            m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::pop, nullptr });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::executeRequests() -> void {
        if (m_action_queue.empty()) return;

        while (!m_action_queue.empty()) {
            auto &caction = m_action_queue.front();

            switch (caction.type) {
                case StateManagerAction::Type::push:
                    if (!m_stack.empty()) m_stack.top()->pause();
                    m_stack.push(std::move(caction.state));
                    break;
                case StateManagerAction::Type::pop:
                    m_stack.pop();
                    if (!m_stack.empty()) m_stack.top()->resume();
                    break;
                case StateManagerAction::Type::set:
                    if (!m_stack.empty()) m_stack.pop();
                    m_stack.push(std::move(caction.state));
                    break;
                case StateManagerAction::Type::clear:
                    if (!m_stack.empty()) {
                        std::stack<std::unique_ptr<State>> q;
                        m_stack.swap(q);
                    }
                    break;
            }

            m_action_queue.pop();
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::update(Secondf delta) -> void {
        if (!m_stack.empty()) m_stack.top()->update(delta);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::requestClear() -> void {
        if (!m_stack.empty())
            m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::clear, nullptr });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::top() noexcept -> State & {
        STORMKIT_EXPECTS(!m_stack.empty());

        return *(m_stack.top());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto StateManager::top() const noexcept -> const State & {
        STORMKIT_EXPECTS(!m_stack.empty());

        return *(m_stack.top());
    }
} // namespace stormkit::core
