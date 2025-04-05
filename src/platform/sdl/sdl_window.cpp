#include "canyon.h"
#include "platform/sdl/sdl_window.h"
#include "graphics/sdl/sdl_surface_context.h"
#include "platform/sdl/sdl_events.h"
#include "graphics/sdl/sdl_graphics.h"
#include <SDL_events.h>
#include <SDL_video.h>
#include <moth_ui/context.h>

namespace platform::sdl {
    bool GetSDLEventForWindow(uint32_t windowId, SDL_Event* outEvent) {
        static std::list<SDL_Event> events;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            events.push_back(event);
        }

        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->window.windowID == windowId) {
                *outEvent = *it;
                events.erase(it);
                return true;
            }
        }

        return false;
    }

    Window::Window(graphics::sdl::Context& context, std::string const& title, int width, int height)
        : platform::Window(title, width, height)
        , m_context(context) {
        CreateWindow();
        Finalize();
    }

    Window::~Window() {
        DestroyWindow();
    }

    void Window::Update(uint32_t ticks) {
        SDL_Event event;
        while (GetSDLEventForWindow(m_windowId, &event)) {
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
        if (nullptr == (m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))) {
            return false;
        }

        if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
            return false;
        }

        m_surfaceContext = std::make_unique<graphics::sdl::SurfaceContext>(m_context, m_renderer);

        m_graphics = std::make_unique<graphics::sdl::Graphics>(*m_surfaceContext);
        m_layerStack = std::make_unique<LayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);

        m_windowId = SDL_GetWindowID(m_window);

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
    }
}
