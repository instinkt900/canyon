#pragma once

#include "moth_ui/event_listener.h"
#include "moth_ui/event_dispatch.h"

class EventEmitter {
public:
    EventEmitter() {}
    virtual ~EventEmitter() {}

    void AddEventListener(moth_ui::EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void RemoveEventListener(moth_ui::EventListener* listener) {
        auto it = std::find(std::begin(m_listeners), std::end(m_listeners), listener);
        if (std::end(m_listeners) != it) {
            m_listeners.erase(it);
        }
    }

    bool EmitEvent(moth_ui::Event const& event) {
        moth_ui::EventDispatch dispatch(event);
        for (auto listener : m_listeners) {
            dispatch.Dispatch(listener);
        }
        return dispatch.GetHandled();
    }

protected:
    std::vector<moth_ui::EventListener*> m_listeners;
};
