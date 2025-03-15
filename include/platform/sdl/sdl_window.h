#pragma once

#include "events/event_window.h"
#include "platform/sdl/sdl_platform.h"
#include "platform/window.h"

namespace platform::sdl {
    class Window : public platform::Window {
    public:
        Window(platform::sdl::Platform& platform, std::string const& applicationTitle, int width, int height);
        virtual ~Window();

        void SetWindowTitle(std::string const& title) override;

        SDL_Window* GetSDLWindow() const { return m_window; }
        SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        platform::sdl::Platform& m_platform;
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;

        bool OnResizeEvent(EventWindowSize const& event);
    };
}

