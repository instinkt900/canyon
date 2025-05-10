#pragma once

#include <moth_ui/events/event_listener.h>
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/events/event.h>

#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

namespace canyon {
    class LambdaHandle {
        friend class EventEmitter;

    public:
        LambdaHandle() = default;
        bool IsValid() const { return m_id != 0; }

    private:
        explicit LambdaHandle(uint32_t id)
            : m_id(id) {}
        uint32_t m_id = 0;
    };

    class LambdaWrapper : public moth_ui::EventListener {
    public:
        LambdaWrapper(std::function<bool(moth_ui::Event const&)> const& lambda, uint32_t id)
            : m_lambda(lambda)
            , m_id(id) {
        }

        virtual ~LambdaWrapper() = default;

        uint32_t GetID() const { return m_id; }

        bool OnEvent(moth_ui::Event const& event) override {
            return m_lambda(event);
        }

    private:
        std::function<bool(moth_ui::Event const&)> m_lambda;
        uint32_t m_id;
    };

    class EventEmitter {
    public:
        EventEmitter() = default;
        virtual ~EventEmitter() = default;

        void AddEventListener(moth_ui::EventListener* listener) {
            m_listeners.push_back(listener);
        }

        void RemoveEventListener(moth_ui::EventListener* listener) {
            m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
        }

        LambdaHandle AddEventListener(std::function<bool(moth_ui::Event const&)> const& lambda) {
            m_owning.emplace_back(std::make_unique<LambdaWrapper>(lambda, ++m_nextLambdaId));
            AddEventListener(m_owning.back().get());
            return LambdaHandle(m_owning.back()->GetID());
        }

        void RemoveEventListener(LambdaHandle const& handle) {
            if (!handle.IsValid())
                return;

            auto it = std::find_if(m_owning.begin(), m_owning.end(), [&](std::unique_ptr<LambdaWrapper> const& ptr) {
                return ptr->GetID() == handle.m_id;
            });

            if (it != m_owning.end()) {
                RemoveEventListener(it->get());
                m_owning.erase(it);
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
        uint32_t m_nextLambdaId = 0;
        std::vector<moth_ui::EventListener*> m_listeners;
        std::vector<std::unique_ptr<LambdaWrapper>> m_owning;
    };
}
