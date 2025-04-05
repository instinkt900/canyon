#pragma once

#include "moth_ui/event_listener.h"
#include "moth_ui/event_dispatch.h"
#include <moth_ui/events/event.h>

class LambdaWrapper : public moth_ui::EventListener {
    public:
        LambdaWrapper(std::function<bool(moth_ui::Event const&)> const& lambda)
            :m_lambda(lambda) {
            }

        bool OnEvent(moth_ui::Event const& event) override {
            return m_lambda(event);
        }

    private:
        std::function<bool(moth_ui::Event const&)> m_lambda;
};

class EventEmitter {
public:
    EventEmitter() {}
    virtual ~EventEmitter() {}

    void AddEventListener(std::function<bool(moth_ui::Event const&)> const& lambda) {
        m_owning.emplace_back(std::make_unique<LambdaWrapper>(lambda));
        AddEventListener(m_owning.back().get());
    }

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
    std::vector<std::unique_ptr<LambdaWrapper>> m_owning;
};
