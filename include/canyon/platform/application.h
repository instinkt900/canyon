#pragma once

#include "canyon/platform/iplatform.h"
#include "canyon/utils/ticker.h"
#include "canyon/events/event_window.h"
#include "canyon/utils/vector.h"

#include <moth_ui/events/event_listener.h>
#include <moth_ui/events/event.h>

#include <memory>
#include <cstdint>
#include <string>

namespace canyon::platform {
    class Application : public Ticker, public moth_ui::EventListener {
    public:
        Application(IPlatform& platform, std::string const& title, int width, int height);
        virtual ~Application() = default;

        void Init();
        void Run();

        bool OnEvent(moth_ui::Event const& event) override;
        bool OnWindowSizeEvent(EventWindowSize const& event);
        bool OnRequestQuitEvent(EventRequestQuit const& event);
        bool OnQuitEvent(EventQuit const& event);

        void TickFixed(uint32_t ticks) override;
        void Tick(uint32_t ticks) override;

        Window* GetWindow() { return m_window.get(); }

    protected:
        virtual void Startup() {}
        virtual void PostCreateWindow() {}
        virtual void Shutdown() {}

        IPlatform& m_platform;
        std::string m_mainWindowTitle;
        int m_mainWindowWidth;
        int m_mainWindowHeight;
        IntVec2 m_mainWindowPosition = { 0, 0 };
        bool m_mainWindowMaximized = false;
        std::unique_ptr<Window> m_window;
    };
}
