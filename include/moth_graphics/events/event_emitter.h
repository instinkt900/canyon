#pragma once

#include <moth_ui/events/event_listener.h>
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/events/event.h>

#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

namespace moth_graphics {
    /// @brief Opaque handle to a lambda-based event listener registration.
    ///
    /// Returned by @c EventEmitter::AddEventListener(lambda). Pass back to
    /// @c EventEmitter::RemoveEventListener() to unregister the listener.
    class LambdaHandle {
        friend class EventEmitter;

    public:
        LambdaHandle() = default;

        /// @brief Returns @c true if this handle was assigned by
        /// @c EventEmitter::AddEventListener (i.e. non-default-constructed).
        /// Does @e not reflect whether the listener is still registered;
        /// a handle remains non-zero after @c RemoveEventListener() is called.
        bool IsValid() const { return id != 0; }

    private:
        explicit LambdaHandle(uint32_t id)
            : id(id) {}
        uint32_t id = 0;
    };

    /// @internal Wraps a callable as an @c IEventListener so it can be stored in
    /// the listeners list.
    class LambdaWrapper : public moth_ui::IEventListener {
    public:
        LambdaWrapper(std::function<bool(moth_ui::Event const&)> const& lambda, uint32_t id)
            : m_lambda(lambda)
            , id(id) {
        }

        ~LambdaWrapper() override = default;

        uint32_t GetID() const { return id; }

        bool OnEvent(moth_ui::Event const& event) override {
            return m_lambda(event);
        }

    private:
        std::function<bool(moth_ui::Event const&)> m_lambda;
        uint32_t id;
    };

    /// @brief Broadcasts events to a list of registered listeners.
    ///
    /// Listeners can be registered as raw @c IEventListener pointers (caller
    /// manages lifetime) or as lambdas (lifetime managed by this emitter via a
    /// @c LambdaHandle).
    class EventEmitter {
    public:
        EventEmitter() = default;
        virtual ~EventEmitter() = default;

        /// @brief Register a listener. The caller must ensure @p listener outlives this emitter.
        void AddEventListener(moth_ui::IEventListener* listener) {
            m_listeners.push_back(listener);
        }

        /// @brief Unregister a previously added pointer listener.
        void RemoveEventListener(moth_ui::IEventListener* listener) {
            m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
        }

        /// @brief Register a lambda as a listener. The emitter owns the lambda's lifetime.
        /// @returns A handle that can be used to unregister the listener later.
        LambdaHandle AddEventListener(std::function<bool(moth_ui::Event const&)> const& lambda) {
            m_owning.emplace_back(std::make_unique<LambdaWrapper>(lambda, ++m_nextLambdaId));
            AddEventListener(m_owning.back().get());
            return LambdaHandle(m_owning.back()->GetID());
        }

        /// @brief Unregister a lambda listener by its handle.
        void RemoveEventListener(LambdaHandle const& handle) {
            if (!handle.IsValid()) {
                return;
            }

            auto it = std::find_if(m_owning.begin(), m_owning.end(), [&](std::unique_ptr<LambdaWrapper> const& ptr) {
                return ptr->GetID() == handle.id;
            });

            if (it != m_owning.end()) {
                RemoveEventListener(it->get());
                m_owning.erase(it);
            }
        }

        /// @brief Dispatch @p event to all registered listeners.
        /// @returns @c true if any listener handled the event.
        bool EmitEvent(moth_ui::Event const& event) {
            moth_ui::EventDispatch dispatch(event);
            std::vector<moth_ui::IEventListener*> snapshot(m_listeners);
            for (auto* listener : snapshot) {
                // Listener may have been removed during a prior callback.
                if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) {
                    continue;
                }
                dispatch.Dispatch(listener);
            }
            return dispatch.GetHandled();
        }

    protected:
        uint32_t m_nextLambdaId = 0;
        std::vector<moth_ui::IEventListener*> m_listeners;
        std::vector<std::unique_ptr<LambdaWrapper>> m_owning;
    };
}
