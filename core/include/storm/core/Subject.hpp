// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <queue>
#include <stack>
#include <storm/core/Memory.hpp>
#include <storm/core/Observer.hpp>
#include <unordered_map>
#include <vector>

namespace storm::core {
    template<typename Event, typename CustomData, bool lifo = false>
    class Subject {
      public:
        using ObserverType = Observer<Event, CustomData>;
        using ObserverRef  = std::reference_wrapper<ObserverType>;

        Subject();

        Subject(const Subject &subject) = default;
        Subject(Subject &&subject)      = default;

        Subject &operator=(const Subject &subject) = default;
        Subject &operator=(Subject &&subject) = default;

        void registerObserver(ObserverType &observer);

        void notify(Event &&event, CustomData &&args);
        void notifyDefferedEvents();

        inline void resetObserver();
        inline ObserverType &observer() noexcept;
        inline const ObserverType &observer() const noexcept;
        inline bool hasObserver() const noexcept;

        void defferEvent(Event &&event, CustomData &&args);

      private:
        using DataType      = std::pair<Event, CustomData>;
        using FifoType      = std::queue<std::pair<Event, CustomData>>;
        using LifoType      = std::stack<std::pair<Event, CustomData>>;
        using ContainerType = std::conditional_t<lifo, LifoType, FifoType>;

        inline DataType &getDatas();

        ObserverType *m_observer = nullptr;

        ContainerType m_deffered_events;
    };
} // namespace storm::core

#include "Subject.inl"
