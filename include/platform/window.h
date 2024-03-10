#pragma once

#include "events/event_emitter.h"
#include "events/event_window.h"
#include "utils/vector.h"

namespace platform {
    class Window : public EventEmitter {
    public:
        Window(std::string const& windowTitle, int width, int height);
        virtual ~Window();

        virtual void Update() {}
        virtual void Draw() {}

        virtual void SetWindowTitle(std::string const& title) = 0;

        int GetWidth() const { return m_windowWidth; }
        int GetHeight() const { return m_windowHeight; }

    protected:
        virtual bool CreateWindow() = 0;
        virtual void DestroyWindow() = 0;

        std::string m_title;
        int m_windowWidth = 0;
        int m_windowHeight = 0;
        IntVec2 m_windowPos = { -1, -1 };
        bool m_windowMaximized = false;
    };
}
