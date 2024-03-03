#pragma once

#include "events/event_listener.h"
#include "events/event_window.h"
#include "utils/vector.h"
#include <string>
#include <chrono>

#undef CreateWindow

class Application : public EventListener {
public:
    Application(std::string const& applicationTitle);
    virtual ~Application();

    virtual void SetWindowTitle(std::string const& title) = 0;

    int Run();
    void Stop() { m_running = false; }

    virtual bool OnEvent(Event const& event);

protected:
    virtual bool CreateWindow() = 0;
    virtual void DestroyWindow() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    static int constexpr INIT_WINDOW_WIDTH = 640;
    static int constexpr INIT_WINDOW_HEIGHT = 480;

    std::string m_applicationTitle;
    std::string m_imguiSettingsPath;
    IntVec2 m_windowPos = { -1, -1 };
    int m_windowWidth = 0;
    int m_windowHeight = 0;
    bool m_windowMaximized = false;

    bool m_running = false;
};
