// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    void Subject<Event, CustomData, lifo>::resetObserver() {
        m_observer = nullptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    typename Subject<Event, CustomData, lifo>::ObserverType &
        Subject<Event, CustomData, lifo>::observer() noexcept {
        return *m_observer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    const typename Subject<Event, CustomData, lifo>::ObserverType &
        Subject<Event, CustomData, lifo>::observer() const noexcept {
        return *m_observer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    bool Subject<Event, CustomData, lifo>::observer() const noexcept {
        return m_observer != nullptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    Subject<Event, CustomData, lifo>::Subject()
        : m_observer(nullptr) {

          };

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    void Subject<Event, CustomData, lifo>::registerObserver(ObserverType &observer) {
        m_observer = &observer;

        notifyDefferedEvents();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    void Subject<Event, CustomData, lifo>::notify(Event &&event, CustomData &&args) {
        if (m_observer) m_observer->onNotified(event, std::move(args));
        else
            defferEvent(std::move(event), std::move(args));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    void Subject<Event, CustomData, lifo>::notifyDefferedEvents() {
        while (!m_deffered_events.empty()) {
            auto &pair = getDatas();

            // if(pair.first != nullptr)
            m_observer->onNotified(pair.first, std::move(pair.second));

            m_deffered_events.pop();
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    void Subject<Event, CustomData, lifo>::defferEvent(Event &&event, CustomData &&args) {
        m_deffered_events.emplace(std::move(event), std::move(args));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Event, typename CustomData, bool lifo>
    typename Subject<Event, CustomData, lifo>::DataType &
        Subject<Event, CustomData, lifo>::getDatas() {
        if constexpr (lifo) return m_deffered_events.top();
        else
            return m_deffered_events.front();
    }
} // namespace storm::core
