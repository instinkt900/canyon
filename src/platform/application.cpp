#include "common.h"
#include "canyon/platform/application.h"
#include "canyon/platform/window.h"

namespace canyon::platform {
    Application::Application(platform::IPlatform& platform, std::string const& title, int width, int height)
        : m_platform(platform)
        , m_mainWindowTitle(title)
        , m_mainWindowWidth(width)
        , m_mainWindowHeight(height) {

    }

    void Application::Run() {
        Startup();
        m_window = m_platform.CreateWindow(m_mainWindowTitle, m_mainWindowWidth, m_mainWindowHeight);
        m_window->AddEventListener(this);
        PostCreateWindow();
        TickSync();
        Shutdown();
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
}
