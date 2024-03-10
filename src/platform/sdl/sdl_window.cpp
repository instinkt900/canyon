#include "canyon.h"
#include "platform/sdl/sdl_window.h"
#include "platform/sdl/sdl_events.h"

namespace platform::sdl {
    Window::Window(std::string const& title, int width, int height)
        :platform::Window(title, width, height) {
            CreateWindow();
    }

    Window::~Window() {
        DestroyWindow();
    }

    void Window::Update() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (auto const translatedEvent = FromSDL(event)) {
                EmitEvent(*translatedEvent);
            }
        }
    }

    bool Window::CreateWindow() {
        if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
            return false;
        }

        if (nullptr == (m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))) {
            return false;
        }

        if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
            return false;
        }
        
        return true;
    }

    void Window::SetWindowTitle(std::string const& title) {
        m_title = title;
        SDL_SetWindowTitle(m_window, m_title.c_str());
    }

    void Window::Draw() {
        SDL_RenderPresent(m_renderer);
    }

    void Window::DestroyWindow() {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }
}
