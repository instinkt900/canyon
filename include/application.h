#pragma once

#include "layers/layer_stack.h"
#include "events/event_window.h"
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
    virtual void SetupLayers() = 0;
    virtual void UpdateWindow() = 0;
    virtual void Draw() = 0;

    void Update();

    bool OnWindowSizeEvent(EventWindowSize const& event);
    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    static int constexpr INIT_WINDOW_WIDTH = 1280;
    static int constexpr INIT_WINDOW_HEIGHT = 960;

    std::string m_applicationTitle;
    std::string m_imguiSettingsPath;
    IntVec2 m_windowPos = { -1, -1 };
    int m_windowWidth = 0;
    int m_windowHeight = 0;
    bool m_windowMaximized = false;

    bool m_running = false;
    bool m_paused = false;
    std::chrono::milliseconds m_updateTicks;
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;

    std::unique_ptr<LayerStack> m_layerStack;
};
