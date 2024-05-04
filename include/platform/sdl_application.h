#pragma once

#include "ticker.h"
#include "moth_ui//event_listener.h"
#include "platform/sdl/sdl_window.h"
#include "graphics/sdl/sdl_graphics.h"
#include "layers/layer_stack.h"
#include "events/event_window.h"
#include <moth_ui/font_factory.h>
#include <moth_ui/iimage_factory.h>
#include <moth_ui/irenderer.h>

class SDLApplication : public Ticker, public moth_ui::EventListener {
public:
    SDLApplication();

    bool OnEvent(moth_ui::Event const& event) override;
    bool OnWindowSizeEvent(EventWindowSize const& event);
    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    void Tick(uint32_t ticks) override;

    LayerStack& GetLayerStack() { return *m_layerStack; }
    graphics::IGraphics& GetGraphics() { return *m_graphics; }
    platform::sdl::Window& GetWindow() { return *m_window; }

private:
    std::unique_ptr<platform::sdl::Window> m_window;
    std::unique_ptr<graphics::sdl::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;

    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::FontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;
};
