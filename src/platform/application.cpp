#include "canyon.h"
#include "platform/application.h"
#include "moth_ui/context.h"

Application::Application(platform::IPlatform& platform)
    : m_platform(platform) {

    m_window = m_platform.CreateWindow("testing", 640, 480);
    m_window->AddEventListener(this);
    m_window->GetLayerStack().AddEventListener(this);
    
    auto& context = m_platform.GetGraphicsContext();
    m_imageFactory = std::make_unique<graphics::ImageFactory>(context);
    m_fontFactory = std::make_unique<graphics::FontFactory>(context);

    auto& graphics = m_window->GetGraphics();
    m_uiRenderer = std::make_unique<graphics::MothRenderer>(graphics);

    m_mothImageFactory = std::make_unique<graphics::MothImageFactory>(m_imageFactory);
    m_mothFontFactory = std::make_unique<graphics::MothFontFactory>(m_fontFactory);

    auto uiContext = std::make_shared<moth_ui::Context>(m_mothImageFactory.get(), m_mothFontFactory.get(), m_uiRenderer.get());
    moth_ui::Context::SetCurrentContext(uiContext);
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
