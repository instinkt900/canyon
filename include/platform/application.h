#pragma once

#include "platform/iplatform.h"
#include "utils/ticker.h"
#include "events/event_window.h"

#include <moth_ui//event_listener.h>
#include <moth_ui/ifont_factory.h>
#include <moth_ui/iimage_factory.h>
#include <moth_ui/irenderer.h>

#include <memory>

class Application : public Ticker, public moth_ui::EventListener {
public:
    Application(platform::IPlatform& platform);

    bool OnEvent(moth_ui::Event const& event) override;
    bool OnWindowSizeEvent(EventWindowSize const& event);
    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    void Tick(uint32_t ticks) override;

    platform::Window& GetWindow() { return *m_window; }

private:
    platform::IPlatform& m_platform;

    std::unique_ptr<platform::Window> m_window;

};
