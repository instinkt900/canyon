#include "common.h"
#include "canyon/platform/sdl/sdl_window.h"
#include "canyon/graphics/sdl/sdl_surface_context.h"
#include "canyon/platform/sdl/sdl_events.h"
#include "canyon/graphics/sdl/sdl_graphics.h"

namespace {
    std::mutex EventFetchMutex;
    std::list<SDL_Event> PendingEvents;

    bool CollectSDLEventsForWindow(uint32_t windowId, std::vector<SDL_Event>* outEvents) {
        std::lock_guard lock(EventFetchMutex);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            PendingEvents.push_back(event);
        }

        bool found_event = false;
        for (auto it = PendingEvents.begin(); it != PendingEvents.end(); /* manually iterate */) {
            if (it->window.windowID == windowId) {
                outEvents->push_back(*it);
                found_event = true;
                it = PendingEvents.erase(it);
            } else {
                ++it;
            }
        }

        return found_event;
    }
}

namespace platform::sdl {
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
        std::vector<SDL_Event> events;
        CollectSDLEventsForWindow(m_windowId, &events);
        for (auto event : events) {
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
