#include "platform/sdl/sdl_app.h"
#include "platform/sdl/sdl_events.h"

SDLApplication::SDLApplication(std::string const& applicationTitle)
    :Application(applicationTitle) {
}

SDLApplication::~SDLApplication() {
}

void SDLApplication::UpdateWindow() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (auto const translatedEvent = FromSDL(event)) {
            OnEvent(*translatedEvent);
        }
    }
}

bool SDLApplication::CreateWindow() {
    if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
        return false;
    }

    if (nullptr == (m_window = SDL_CreateWindow(m_applicationTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))) {
        return false;
    }

    if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
        return false;
    }

    return true;
}

void SDLApplication::SetWindowTitle(std::string const& title) {
    m_applicationTitle = title;
    SDL_SetWindowTitle(m_window, m_applicationTitle.c_str());
}

void SDLApplication::Draw() {
    m_layerStack->Draw();
    SDL_RenderPresent(m_renderer);
}

void SDLApplication::DestroyWindow() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void SDLApplication::SetupLayers() {
    
}