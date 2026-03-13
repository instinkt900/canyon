#pragma once

#include "canyon/events/canyon_events.h"

#include <moth_ui/events/event.h>

#include <memory>

namespace canyon {
    /// @brief Fired when the render device is lost and recreated (e.g. GPU reset).
    class EventRenderDeviceReset : public moth_ui::Event {
    public:
        EventRenderDeviceReset()
            : Event(GetStaticType()) {}
        virtual ~EventRenderDeviceReset() = default;

        static constexpr int GetStaticType() { return EVENTTYPE_RENDERDEVICERESET; }

        std::unique_ptr<Event> Clone() const override {
            return std::make_unique<EventRenderDeviceReset>();
        }
    };

    /// @brief Fired when a render target is invalidated and must be recreated.
    class EventRenderTargetReset : public moth_ui::Event {
    public:
        EventRenderTargetReset()
            : Event(GetStaticType()) {}
        virtual ~EventRenderTargetReset() = default;

        static constexpr int GetStaticType() { return EVENTTYPE_RENDERTARGETRESET; }

        std::unique_ptr<Event> Clone() const override {
            return std::make_unique<EventRenderTargetReset>();
        }
    };

    /// @brief Fired when a window is resized.
    class EventWindowSize : public moth_ui::Event {
    public:
        /// @param width New window width in pixels.
        /// @param height New window height in pixels.
        EventWindowSize(int width, int height)
            : Event(GetStaticType())
            , m_width(width)
            , m_height(height) {}
        virtual ~EventWindowSize() = default;

        static constexpr int GetStaticType() { return EVENTTYPE_WINDOWSIZE; }

        /// @brief Returns the new window width in pixels.
        int GetWidth() const { return m_width; }

        /// @brief Returns the new window height in pixels.
        int GetHeight() const { return m_height; }

        std::unique_ptr<Event> Clone() const override {
            return std::make_unique<EventWindowSize>(m_width, m_height);
        }

    private:
        int m_width = 0;
        int m_height = 0;
    };

    /// @brief Fired when the user or system requests the application to close
    ///        (e.g. clicking the window close button).
    ///
    /// Handlers may ignore this event to veto the close. If no handler vetoes it,
    /// @c Application will stop the main loop.
    class EventRequestQuit : public moth_ui::Event {
    public:
        EventRequestQuit()
            : Event(GetStaticType()) {}
        virtual ~EventRequestQuit() = default;

        static constexpr int GetStaticType() { return EVENTTYPE_REQUEST_QUIT; }

        std::unique_ptr<Event> Clone() const override {
            return std::make_unique<EventRequestQuit>();
        }
    };

    /// @brief Fired to unconditionally terminate the application.
    class EventQuit : public moth_ui::Event {
    public:
        EventQuit()
            : Event(GetStaticType()) {}
        virtual ~EventQuit() = default;

        static constexpr int GetStaticType() { return EVENTTYPE_QUIT; }

        std::unique_ptr<Event> Clone() const override {
            return std::make_unique<EventQuit>();
        }
    };
}
