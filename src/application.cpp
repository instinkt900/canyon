#include "application.h"
#include "events/event_dispatch.h"

Application::Application(std::string const& applicationTitle)
    : m_applicationTitle(applicationTitle)
    , m_windowWidth(INIT_WINDOW_WIDTH)
    , m_windowHeight(INIT_WINDOW_HEIGHT) {
    m_updateTicks = std::chrono::milliseconds(1000 / 60);
}

Application::~Application() {
}

int Application::Run() {
    if (!CreateWindow()) {
        return 1;
    }

    m_layerStack = std::make_unique<LayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
    m_layerStack->SetEventListener(this);

    m_running = true;
    m_lastUpdateTicks = std::chrono::steady_clock::now();

    while (m_running) {
        UpdateWindow();
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
    dispatch.Dispatch(this, &Application::OnWindowSizeEvent);
    dispatch.Dispatch(this, &Application::OnRequestQuitEvent);
    dispatch.Dispatch(this, &Application::OnQuitEvent);
    dispatch.Dispatch(m_layerStack.get());
    return dispatch.GetHandled();
}

void Application::Update() {
    auto const nowTicks = std::chrono::steady_clock::now();
    auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
    while (deltaTicks > m_updateTicks) {
        if (!m_paused) {
            m_layerStack->Update(static_cast<uint32_t>(m_updateTicks.count()));
        }
        m_lastUpdateTicks += m_updateTicks;
        deltaTicks -= m_updateTicks;
    }
}

bool Application::OnWindowSizeEvent(EventWindowSize const& event) {
    m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
    return true;
}

bool Application::OnRequestQuitEvent(EventRequestQuit const& event) {
    m_running = false;
    return true;
}

bool Application::OnQuitEvent(EventQuit const& event) {
    m_running = false;
    return true;
}
