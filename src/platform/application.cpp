#include "canyon.h"
#include "platform/application.h"
#include "platform/window.h"

Application::Application(platform::IPlatform& platform)
    : m_platform(platform) {

    m_window = m_platform.CreateWindow("testing", 640, 480);
    m_window->AddEventListener(this);
    
}

bool Application::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &Application::OnWindowSizeEvent);
    dispatch.Dispatch(&m_window->GetLayerStack());
    dispatch.Dispatch(this, &Application::OnRequestQuitEvent);
    dispatch.Dispatch(this, &Application::OnQuitEvent);
    return dispatch.GetHandled();
}

bool Application::OnWindowSizeEvent(EventWindowSize const& event) {
    return true;
}

bool Application::OnRequestQuitEvent(EventRequestQuit const& event) {
    SetRunning(false);
    return true;
}

bool Application::OnQuitEvent(EventQuit const& event) {
    SetRunning(false);
    return true;
}

void Application::Tick(uint32_t ticks) {
    m_window->Update(ticks);
    m_window->GetGraphics().Begin();
    m_window->Draw();
    m_window->GetGraphics().End();
}
