#pragma once

#include "ticker.h"
#include "events/event_listener.h"
#include "platform/glfw/glfw_window.h"
#include "graphics/vulkan/vulkan_context.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include "layers/layer_stack.h"

class VulkApplication : public Ticker, public EventListener {
public:
    VulkApplication();

    bool OnEvent(Event const& event) override;
    bool OnWindowSizeEvent(EventWindowSize const& event);
    bool OnRequestQuitEvent(EventRequestQuit const& event);
    bool OnQuitEvent(EventQuit const& event);

    void Tick(uint32_t ticks) override;

private:
    std::unique_ptr<platform::glfw::Window> m_window;
    std::unique_ptr<graphics::vulkan::Context> m_context;
    VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;
    std::unique_ptr<graphics::vulkan::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
};
