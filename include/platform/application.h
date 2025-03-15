#pragma once

#include "graphics/font_factory.h"
#include "graphics/image_factory.h"
#include "graphics/moth_ui/ui_renderer.h"
#include "platform/iplatform.h"
#include "utils/ticker.h"
#include "moth_ui//event_listener.h"
#include "events/event_window.h"
#include <moth_ui/ifont_factory.h>
#include <moth_ui/iimage_factory.h>
#include <moth_ui/irenderer.h>
#include "graphics/moth_ui/moth_image_factory.h"
#include "graphics/moth_ui/moth_font_factory.h"

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

    std::shared_ptr<graphics::ImageFactory> m_imageFactory;
    std::shared_ptr<graphics::FontFactory> m_fontFactory;

    std::unique_ptr<graphics::MothImageFactory> m_mothImageFactory;
    std::unique_ptr<graphics::MothFontFactory> m_mothFontFactory;
    std::unique_ptr<graphics::UIRenderer> m_uiRenderer;
};
