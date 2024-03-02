#include "application.h"
#include "events/event_dispatch.h"

Application::Application(std::string const& applicationTitle)
    : m_applicationTitle(applicationTitle)
    , m_windowWidth(INIT_WINDOW_WIDTH)
    , m_windowHeight(INIT_WINDOW_HEIGHT) {
}

Application::~Application() {
}

int Application::Run() {
    if (!CreateWindow()) {
        return 1;
    }

    m_running = true;

    while (m_running) {
        Update();
        if (m_windowWidth > 0 && m_windowHeight > 0) {
            Draw();
        }
    }

    DestroyWindow();
    return 0;
}

bool Application::OnEvent(Event const& event) {
    EventDispatch dispatch(event);
    dispatch.Dispatch(this, &Application::OnRequestQuitEvent);
    dispatch.Dispatch(this, &Application::OnQuitEvent);
    return dispatch.GetHandled();
}

bool Application::OnRequestQuitEvent(EventRequestQuit const& event) {
    m_running = false;
    return true;
}

bool Application::OnQuitEvent(EventQuit const& event) {
    m_running = false;
    return true;
}

