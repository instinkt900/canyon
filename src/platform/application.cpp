#include "common.h"
#include "moth_graphics/platform/application.h"
#include "moth_graphics/platform/imgui_context.h"
#include "moth_graphics/platform/window.h"

#include <stdexcept>

namespace moth_graphics::platform {
    Application::Application(platform::IPlatform& platform, std::string_view title, int width, int height)
        : m_platform(platform)
        , m_mainWindowTitle(title)
        , m_mainWindowWidth(width)
        , m_mainWindowHeight(height) {

    }

    void Application::Init() {
        spdlog::info("Application: initializing");
        Startup();
        spdlog::info("Application: creating window '{}' ({}x{})", m_mainWindowTitle, m_mainWindowWidth, m_mainWindowHeight);
        try {
            m_window = m_platform.CreateWindow(m_mainWindowTitle, m_mainWindowWidth, m_mainWindowHeight);
        } catch (std::exception const& e) {
            spdlog::error("Application: failed to create window: {}", e.what());
            throw;
        }
        m_window->AddEventListener(this);
        m_imguiContext = m_platform.CreateImGuiContext();
        if (!m_imguiContext->Init(*m_window, m_window->GetGraphics(), m_imguiViewportsEnabled)) {
            spdlog::error("Application: ImGui context initialization failed");
            throw std::runtime_error("ImGui context initialization failed");
        }
        PostCreateWindow();
        spdlog::info("Application: ready");
    }

    void Application::Run() {
        spdlog::info("Application: running");
        TickSync();
        spdlog::info("Application: shutting down");
        m_imguiContext->Shutdown();
        m_imguiContext.reset();
        Shutdown();
    }

    bool Application::OnEvent(moth_ui::Event const& event) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch(this, &Application::OnWindowSizeEvent);
        dispatch.Dispatch(this, &Application::OnRequestQuitEvent);
        dispatch.Dispatch(this, &Application::OnQuitEvent);
        return dispatch.GetHandled();
    }

    bool Application::OnWindowSizeEvent(EventWindowSize const& /*event*/) { // NOLINT(readability-convert-member-functions-to-static)
        return false;
    }

    bool Application::OnRequestQuitEvent(EventRequestQuit const& event) {
        SetRunning(false);
        return true;
    }

    bool Application::OnQuitEvent(EventQuit const& event) {
        SetRunning(false);
        return true;
    }

    void Application::TickFixed(uint32_t ticks) {
        m_window->Update(ticks);
    }

    void Application::Tick(uint32_t ticks) {
        m_imguiContext->NewFrame();
        if (m_window->Draw()) {
            m_imguiContext->Render(m_window->GetGraphics());
        }
    }
}
