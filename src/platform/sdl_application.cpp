#include "canyon.h"
#include "platform/sdl_application.h"

SDLApplication::SDLApplication() {
    m_window = std::make_unique<platform::sdl::Window>("testing", 640, 480);
    m_window->AddEventListener(this);
    m_graphics = std::make_unique<graphics::sdl::Graphics>(m_window->GetSDLRenderer());
    m_layerStack = std::make_unique<LayerStack>(m_window->GetWidth(), m_window->GetHeight(), m_window->GetWidth(), m_window->GetHeight());
    m_layerStack->AddEventListener(this);
}

bool SDLApplication::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &SDLApplication::OnWindowSizeEvent);
    dispatch.Dispatch(m_layerStack.get());
    dispatch.Dispatch(this, &SDLApplication::OnRequestQuitEvent);
    dispatch.Dispatch(this, &SDLApplication::OnQuitEvent);
    return dispatch.GetHandled();
}

bool SDLApplication::OnWindowSizeEvent(EventWindowSize const& event) {
    m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
    return true;
}

bool SDLApplication::OnRequestQuitEvent(EventRequestQuit const& event) {
    SetRunning(false);
    return true;
}

bool SDLApplication::OnQuitEvent(EventQuit const& event) {
    SetRunning(false);
    return true;
}

void SDLApplication::Tick(uint32_t ticks) {
    m_window->Update();
    m_layerStack->Update(ticks);
    m_layerStack->Draw();
    m_window->Draw();
}
