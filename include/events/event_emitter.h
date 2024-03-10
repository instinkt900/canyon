#pragma once

#include "events/event_listener.h"
#include "events/event_dispatch.h"

class EventEmitter {
public:
    EventEmitter() {}
    virtual ~EventEmitter() {}

    void AddEventListener(EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void RemoveEventListener(EventListener* listener) {
        auto it = std::find(std::begin(m_listeners), std::end(m_listeners), listener);
        if (std::end(m_listeners) != it) {
            m_listeners.erase(it);
        }
    }

    bool EmitEvent(Event const& event) {
        EventDispatch dispatch(event);
        for (auto listener : m_listeners) {
            dispatch.Dispatch(listener);
        }
        return dispatch.GetHandled();
    }

protected:
    std::vector<EventListener*> m_listeners;
};
