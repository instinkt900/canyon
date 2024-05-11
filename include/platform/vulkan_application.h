#pragma once

#include "utils/ticker.h"
#include "moth_ui/event_listener.h"
#include "platform/glfw/glfw_window.h"
#include "graphics/vulkan/vulkan_context.h"
#include "events/event_window.h"
#include <moth_ui/font_factory.h>
#include <moth_ui/iimage_factory.h>
#include <moth_ui/irenderer.h>

class VulkApplication : public Ticker, public moth_ui::EventListener {
public:
    VulkApplication();

    bool OnEvent(moth_ui::Event const& event) override;
    bool OnWindowSizeEvent(EventWindowSize const& event);
    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    void Tick(uint32_t ticks) override;

    platform::Window& GetWindow() { return *m_window; }

private:
    std::unique_ptr<graphics::vulkan::Context> m_context;
    std::unique_ptr<platform::glfw::Window> m_window;

    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::FontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;
};

