#pragma once

#include "canyon/platform/iplatform.h"
#include "canyon/utils/ticker.h"
#include "canyon/events/event_window.h"

#include <moth_ui/event_listener.h>
#include <moth_ui/events/event.h>

#include <memory>
#include <cstdint>

namespace canyon::platform {
    class Application : public Ticker, public moth_ui::EventListener {
    public:
        Application(IPlatform& platform);

        bool OnEvent(moth_ui::Event const& event) override;
        bool OnWindowSizeEvent(EventWindowSize const& event);
        bool OnRequestQuitEvent(EventRequestQuit const& event);
        bool OnQuitEvent(EventQuit const& event);

        void Tick(uint32_t ticks) override;

        Window& GetWindow() { return *m_window; }

    private:
        IPlatform& m_platform;

        std::unique_ptr<Window> m_window;
    };
}
