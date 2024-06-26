#include "canyon.h"
#include "platform/sdl/sdl_window.h"
#include "platform/sdl/sdl_events.h"
#include "graphics/sdl/sdl_graphics.h"

namespace platform::sdl {
    Window::Window(std::string const& title, int width, int height)
        :platform::Window(title, width, height) {
        CreateWindow();
    }

    Window::~Window() {
        DestroyWindow();
    }

    void Window::Update(uint32_t ticks) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (auto const translatedEvent = FromSDL(event)) {
                moth_ui::EventDispatch dispatch(*translatedEvent);
                dispatch.Dispatch(this, &Window::OnResizeEvent);
                if (!dispatch.GetHandled()) {
                    EmitEvent(*translatedEvent);
                }
            }
        }
        m_layerStack->Update(ticks);
    }

    bool Window::OnResizeEvent(EventWindowSize const& event) {
        m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
        return false;
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

        m_graphics = std::make_unique<graphics::sdl::Graphics>(m_renderer);
        m_layerStack = std::make_unique<LayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
        
        return true;
    }

    void Window::SetWindowTitle(std::string const& title) {
        m_title = title;
        SDL_SetWindowTitle(m_window, m_title.c_str());
    }

    void Window::Draw() {
        m_layerStack->Draw();
        SDL_RenderPresent(m_renderer);
    }

    void Window::DestroyWindow() {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }
}

