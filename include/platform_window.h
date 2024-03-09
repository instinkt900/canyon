#pragma once

#include "events/event_listener.h"
#include "events/event_window.h"
#include "utils/vector.h"
#include <string>
#include <chrono>

#undef CreateWindow

namespace platform {
    class Window : public EventListener {
    public:
        Window(std::string const& windowTitle, int width, int height);
        virtual ~Window();

        virtual void Update() {}
        virtual void Draw() {}

        virtual void SetWindowTitle(std::string const& title) = 0;

        virtual bool OnEvent(Event const& event);

        int GetWidth() const { return m_windowWidth; }
        int GetHeight() const { return m_windowHeight; }

    protected:
        virtual bool CreateWindow() = 0;
        virtual void DestroyWindow() = 0;
        
        bool OnRequestQuitEvent(EventRequestQuit const& event);
        bool OnQuitEvent(EventQuit const& event);

        std::string m_title;
        IntVec2 m_windowPos = { -1, -1 };
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        bool m_windowMaximized = false;
    };
}
