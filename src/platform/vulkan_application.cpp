#include "canyon.h"
#include "platform/vulkan_application.h"

VulkApplication::VulkApplication() {
    m_window = std::make_unique<platform::glfw::Window>("testing", 640, 480);
    m_window->AddEventListener(this);
    m_context = std::make_unique<graphics::vulkan::Context>();
    glfwCreateWindowSurface(m_context->m_vkInstance, m_window->GetGLFWWindow(), nullptr, &m_customVkSurface);
    m_graphics = std::make_unique<graphics::vulkan::Graphics>(*m_context, m_customVkSurface, m_window->GetWidth(), m_window->GetHeight());
    m_layerStack = std::make_unique<LayerStack>(m_window->GetWidth(), m_window->GetHeight(), m_window->GetWidth(), m_window->GetHeight());
    m_layerStack->AddEventListener(this);
}

bool VulkApplication::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &VulkApplication::OnWindowSizeEvent);
    dispatch.Dispatch(this, &VulkApplication::OnRequestQuitEvent);
    dispatch.Dispatch(this, &VulkApplication::OnQuitEvent);
    dispatch.Dispatch(m_layerStack.get());
    return dispatch.GetHandled();
}

bool VulkApplication::OnWindowSizeEvent(EventWindowSize const& event) {
    m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
    m_graphics->OnResize(m_customVkSurface, event.GetWidth(), event.GetHeight());
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
    m_window->Update();
    m_layerStack->Update(ticks);
    m_graphics->Begin();
    m_layerStack->Draw();
    m_window->Draw();
    m_graphics->End();
}

