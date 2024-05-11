#include "canyon.h"
#include "platform/sdl_application.h"
#include "graphics/sdl/sdl_font_factory.h"
#include "graphics/sdl/sdl_image_factory.h"
#include "graphics/sdl/sdl_ui_renderer.h"
#include "moth_ui/context.h"

SDLApplication::SDLApplication() {
    m_window = std::make_unique<platform::sdl::Window>("testing", 640, 480);
    m_window->AddEventListener(this);
    auto renderer = m_window->GetSDLRenderer();
    m_imageFactory = std::make_unique<graphics::sdl::ImageFactory>(*renderer);
    m_fontFactory = std::make_unique<graphics::sdl::FontFactory>(*renderer);
    m_uiRenderer = std::make_unique<graphics::sdl::UIRenderer>(*renderer);
    auto uiContext = std::make_shared<moth_ui::Context>(m_imageFactory.get(), m_fontFactory.get(), m_uiRenderer.get());
    moth_ui::Context::SetCurrentContext(uiContext);
    m_window->GetLayerStack().AddEventListener(this);
}

bool SDLApplication::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &SDLApplication::OnWindowSizeEvent);
    dispatch.Dispatch(&m_window->GetLayerStack());
    dispatch.Dispatch(this, &SDLApplication::OnRequestQuitEvent);
    dispatch.Dispatch(this, &SDLApplication::OnQuitEvent);
    return dispatch.GetHandled();
}

bool SDLApplication::OnWindowSizeEvent(EventWindowSize const& event) {
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
    m_window->Update(ticks);
    m_window->Draw();
}
