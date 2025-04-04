#pragma once

#include "events/event_window.h"
#include <moth_ui/event_listener.h>
#include <moth_ui/event_dispatch.h>

class QuitListener : public moth_ui::EventListener {
public:
    QuitListener(bool& signal)
        : m_signal(signal) {}
    bool OnEvent(moth_ui::Event const& event) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch<EventRequestQuit>([&](EventRequestQuit const& event) {
            m_signal = false;
            return true;
        });
        return dispatch.GetHandled();
    }

private:
    bool& m_signal;
};
