#pragma once

#include "platform/window.h"

namespace platform::sdl {
    class Window : public platform::Window {
    public:
        Window(std::string const& applicationTitle, int width, int height);
        virtual ~Window();

        void SetWindowTitle(std::string const& title) override;

        SDL_Window* GetSDLWindow() const { return m_window; }
        SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

        void Update() override;
        void Draw() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
    };
}
