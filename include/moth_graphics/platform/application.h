#pragma once

#include "moth_graphics/platform/iplatform.h"
#include "moth_graphics/utils/ticker.h"
#include "moth_graphics/events/event_window.h"
#include "moth_graphics/utils/vector.h"

#include <moth_ui/events/event_listener.h>
#include <moth_ui/events/event.h>

#include <cassert>
#include <memory>
#include <cstdint>
#include <string>
#include <string_view>

namespace moth_graphics::platform {
    /// @brief Base class for a moth_graphics application.
    ///
    /// Drives the main loop via @c Ticker, owns the primary @c Window, and
    /// routes platform events to the layer stack. Subclass and override
    /// @c Startup(), @c PostCreateWindow(), and @c Shutdown() to customize
    /// application lifecycle behavior.
    class Application : public Ticker, public moth_ui::IEventListener {
    public:
        /// @param platform The platform backend to use (SDL or GLFW).
        /// @param title Initial window title.
        /// @param width Initial window width in pixels.
        /// @param height Initial window height in pixels.
        Application(IPlatform& platform, std::string_view title, int width, int height);
        ~Application() override = default;

        /// @brief Initialize the platform, create the window, and set up ImGui.
        void Init();

        /// @brief Run the main loop until the application is asked to quit.
        void Run();

        bool OnEvent(moth_ui::Event const& event) override;

        void TickFixed(uint32_t ticks) override;
        void Tick(uint32_t ticks) override;

        /// @brief Returns the primary window, or @c nullptr before @c Init() is called.
        Window* GetWindow() { return m_window.get(); }

        /// @brief Enable or disable ImGui multi-viewport support. Must be called before Init().
        /// When enabled, floating ImGui windows are promoted to native OS windows.
        /// Avoid on tiling window managers (i3, sway) — new OS windows disrupt drag-and-drop.
        void SetImGuiViewportsEnabled(bool enabled) {
            assert(m_imguiContext == nullptr && "SetImGuiViewportsEnabled must be called before Init()");
            m_imguiViewportsEnabled = enabled;
        }

        Application(Application const&) = delete;
        Application& operator=(Application const&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

    private:
        bool OnWindowSizeEvent(EventWindowSize const& event);
        bool OnRequestQuitEvent(EventRequestQuit const& event);
        bool OnQuitEvent(EventQuit const& event);

    protected:
        /// @brief Override to perform setup before the window is created.
        virtual void Startup() {}

        /// @brief Override to perform setup after the window and ImGui are ready.
        virtual void PostCreateWindow() {}

        /// @brief Override to perform teardown after the main loop exits.
        virtual void Shutdown() {}

        IPlatform& m_platform;
        std::string m_mainWindowTitle;
        int m_mainWindowWidth;
        int m_mainWindowHeight;
        IntVec2 m_mainWindowPosition = { 0, 0 };
        bool m_mainWindowMaximized = false;
        bool m_imguiViewportsEnabled = false;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<ImGuiContext> m_imguiContext;
    };
}
