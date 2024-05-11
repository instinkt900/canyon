#include "canyon.h"
#include "graphics/vulkan/vulkan_font_factory.h"
#include "graphics/vulkan/vulkan_image_factory.h"
#include "graphics/vulkan/vulkan_ui_renderer.h"
#include <moth_ui/context.h>
#include "platform/vulkan_application.h"

VulkApplication::VulkApplication() {
    m_context = std::make_unique<graphics::vulkan::Context>();
    m_window = std::make_unique<platform::glfw::Window>(*m_context, "testing", 640, 480);
    m_window->AddEventListener(this);
    m_window->GetLayerStack().AddEventListener(this);
    graphics::vulkan::Graphics& graphics = static_cast<graphics::vulkan::Graphics&>(m_window->GetGraphics());
    m_imageFactory = std::make_unique<graphics::vulkan::ImageFactory>(*m_context, graphics);
    m_fontFactory = std::make_unique<graphics::vulkan::FontFactory>(*m_context, graphics);
    m_uiRenderer = std::make_unique<graphics::vulkan::UIRenderer>(graphics);
    auto uiContext = std::make_shared<moth_ui::Context>(m_imageFactory.get(), m_fontFactory.get(), m_uiRenderer.get());
    moth_ui::Context::SetCurrentContext(uiContext);
}

bool VulkApplication::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &VulkApplication::OnWindowSizeEvent);
    dispatch.Dispatch(this, &VulkApplication::OnRequestQuitEvent);
    dispatch.Dispatch(this, &VulkApplication::OnQuitEvent);
    dispatch.Dispatch(&m_window->GetLayerStack());
    return dispatch.GetHandled();
}

bool VulkApplication::OnWindowSizeEvent(EventWindowSize const& event) {
    return true;
}

bool VulkApplication::OnRequestQuitEvent(EventRequestQuit const& event) {
    SetRunning(false);
    return true;
}

bool VulkApplication::OnQuitEvent(EventQuit const& event) {
    SetRunning(false);
    return true;
}

void VulkApplication::Tick(uint32_t ticks) {
    m_window->Update(ticks);
    m_window->Draw();
}

