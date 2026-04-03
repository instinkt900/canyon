#include "backends/imgui_impl_sdl2.h"
#include "common.h"
#include "moth_graphics/platform/sdl/sdl_window.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/platform/sdl/sdl_events.h"
#include "moth_graphics/graphics/sdl/sdl_graphics.h"

#include <stdexcept>

namespace {
    std::mutex EventFetchMutex;      // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    std::list<SDL_Event> PendingEvents; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    bool IsInputEvent(SDL_Event const& event) {
        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_TEXTINPUT:
        case SDL_TEXTEDITING:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
        case SDL_CONTROLLERAXISMOTION:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            return true;
        default:
            return false;
        }
    }

    // Patch windowID for keyboard/mouse events that SDL reports with windowID == 0
    // (common when no window has focus). SDL2 guarantees windowID sits at the same
    // struct offset across all event types, so event.window.windowID is a valid
    // read regardless of which union member is active.
    void ResolveInputEventWindowID(SDL_Event& event) {
        auto focusedId = [](SDL_Window* w) -> Uint32 { return w ? SDL_GetWindowID(w) : 0; };
        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (event.key.windowID == 0) { event.key.windowID = focusedId(SDL_GetKeyboardFocus()); }
            break;
        case SDL_TEXTINPUT:
            if (event.text.windowID == 0) { event.text.windowID = focusedId(SDL_GetKeyboardFocus()); }
            break;
        case SDL_TEXTEDITING:
            if (event.edit.windowID == 0) { event.edit.windowID = focusedId(SDL_GetKeyboardFocus()); }
            break;
        case SDL_MOUSEMOTION:
            if (event.motion.windowID == 0) { event.motion.windowID = focusedId(SDL_GetMouseFocus()); }
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (event.button.windowID == 0) { event.button.windowID = focusedId(SDL_GetMouseFocus()); }
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.windowID == 0) { event.wheel.windowID = focusedId(SDL_GetMouseFocus()); }
            break;
        default:
            break;
        }
    }

    bool CollectSDLEventsForWindow(uint32_t windowId, std::vector<SDL_Event>* outEvents) {
        std::lock_guard lock(EventFetchMutex);

        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0) {
            if (IsInputEvent(event)) {
                ResolveInputEventWindowID(event);
            }
            // Retain window-targeted events, resolved input events, and SDL_QUIT.
            // Touch/controller events have no windowID and keep windowID == 0.
            if (event.window.windowID != 0 || event.type == SDL_QUIT || IsInputEvent(event)) {
                PendingEvents.push_back(event);
            }
        }

        bool found_event = false;
        for (auto it = PendingEvents.begin(); it != PendingEvents.end(); /* manually iterate */) {
            // Deliver to this window, or broadcast SDL_QUIT and unresolvable input
            // events (touch/controller, windowID still 0 after resolution attempt).
            bool const isUnresolvableInput = IsInputEvent(*it) && it->window.windowID == 0;
            if (it->window.windowID == windowId || it->type == SDL_QUIT || isUnresolvableInput) {
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

namespace moth_graphics::platform::sdl {
    Window::Window(graphics::sdl::Context& context, std::string const& title, int width, int height)
        : platform::Window(title, width, height)
        , m_context(context) {
        if (!CreateWindow()) {
            throw std::runtime_error("SDL: failed to create window '" + title + "'");
        }
        PostCreate();
    }

    Window::~Window() {
        DestroyWindow();
    }

    void Window::Update(uint32_t ticks) {
        std::vector<SDL_Event> events;
        CollectSDLEventsForWindow(m_windowId, &events);
        for (auto event : events) {
            ImGui_ImplSDL2_ProcessEvent(&event);
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
        spdlog::info("SDL: creating window '{}' ({}x{})", m_title, m_windowWidth, m_windowHeight);
        if (nullptr == (m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))) {
            spdlog::error("SDL: failed to create window '{}': {}", m_title, SDL_GetError());
            return false;
        }

        if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
            spdlog::error("SDL: failed to create renderer: {}", SDL_GetError());
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            return false;
        }

        m_surfaceContext = std::make_unique<graphics::sdl::SurfaceContext>(m_context, m_renderer);
        m_graphics = std::make_unique<graphics::sdl::Graphics>(*m_surfaceContext);
        m_windowId = SDL_GetWindowID(m_window);
        spdlog::info("SDL: window '{}' ready", m_title);
        return true;
    }

    void Window::SetWindowTitle(std::string const& title) {
        m_title = title;
        SDL_SetWindowTitle(m_window, m_title.c_str());
    }

    void Window::Draw() {
        m_graphics->Begin();
        m_layerStack->Draw();
        m_graphics->End();
        SDL_RenderPresent(m_renderer);
    }

    void Window::DestroyWindow() {
        spdlog::info("SDL: destroying window '{}'", m_title);
        PreDestroy();
        m_graphics.reset();
        m_surfaceContext.reset();
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
    }
}
