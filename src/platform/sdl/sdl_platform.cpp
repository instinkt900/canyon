#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_context.h"
#include "moth_graphics/platform/sdl/sdl_window.h"
#include "moth_graphics/platform/sdl/sdl_platform.h"

namespace moth_graphics::platform::sdl {
    graphics::Context& Platform::GetGraphicsContext() {
        // lazy init this will mean the first window created is linked to this context
        if (!m_context) {
            m_context = std::make_unique<graphics::sdl::Context>();
        }
        return *m_context;
    }

    std::unique_ptr<platform::Window> Platform::CreateWindow(std::string_view title, int width, int height) {
        return std::make_unique<platform::sdl::Window>(*m_context, title, width, height);
    }

    bool Platform::Startup() {
        if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
            spdlog::error("SDL: initialization failed: {}", SDL_GetError());
            return false;
        }
        SDL_EnableScreenSaver();
        spdlog::info("SDL: initialized");
        return true;
    }

    void Platform::Shutdown() {
        spdlog::info("SDL: shutting down");
        SDL_Quit();
    }
}
