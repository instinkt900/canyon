#pragma once

#include "canyon/platform/iplatform.h"
#include "canyon/utils/ticker.h"
#include "canyon/events/event_window.h"
#include "canyon/utils/vector.h"

#include <moth_ui/events/event_listener.h>
#include <moth_ui/events/event.h>

#include <memory>
#include <cstdint>
#include <string>

namespace canyon::platform {
    /// @brief Base class for a canyon application.
    ///
    /// Drives the main loop via @c Ticker, owns the primary @c Window, and
    /// routes platform events to the layer stack. Subclass and override
    /// @c Startup(), @c PostCreateWindow(), and @c Shutdown() to customise
    /// application lifecycle behaviour.
    class Application : public Ticker, public moth_ui::EventListener {
    public:
        /// @param platform The platform backend to use (SDL or GLFW).
        /// @param title Initial window title.
        /// @param width Initial window width in pixels.
        /// @param height Initial window height in pixels.
        Application(IPlatform& platform, std::string const& title, int width, int height);
        virtual ~Application() = default;

        /// @brief Initialise the platform, create the window, and set up ImGui.
        void Init();

        /// @brief Run the main loop until the application is asked to quit.
        void Run();

        bool OnEvent(moth_ui::Event const& event) override;

        /// @brief Called when the window is resized. Returns @c false by default.
        bool OnWindowSizeEvent(EventWindowSize const& event);

        /// @brief Called on a quit request (e.g. window close button); stops the loop.
        bool OnRequestQuitEvent(EventRequestQuit const& event);

        /// @brief Called on a hard quit event; stops the loop.
        bool OnQuitEvent(EventQuit const& event);

        void TickFixed(uint32_t ticks) override;
        void Tick(uint32_t ticks) override;

        /// @brief Returns the primary window, or @c nullptr before @c Init() is called.
        Window* GetWindow() { return m_window.get(); }

        Application(Application const&) = delete;
        Application& operator=(Application const&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

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
        std::unique_ptr<Window> m_window;
    };
}
