#include "common.h"
#include "moth_graphics/graphics/sdl/sdl_context.h"
#include "moth_graphics/platform/sdl/sdl_window.h"
#include "moth_graphics/platform/sdl/sdl_platform.h"

namespace moth_graphics::platform::sdl {
    graphics::Context& Platform::GetGraphicsContext() {
        return *m_context;
    }

    std::unique_ptr<platform::Window> Platform::CreateWindow(std::string_view title, int width, int height) {
        return std::make_unique<platform::sdl::Window>(*m_context, title, width, height);
    }

    bool Platform::Startup() {
        SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
        if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
            spdlog::error("SDL: initialization failed: {}", SDL_GetError());
            return false;
        }
        spdlog::info("SDL: initialized");
        m_context = std::make_unique<graphics::sdl::Context>();
        if (!m_context->Startup()) {
            spdlog::error("SDL: graphics context startup failed");
            m_context.reset();
            SDL_Quit();
            return false;
        }
        return true;
    }

    void Platform::Shutdown() {
        spdlog::info("SDL: shutting down");
        if (m_context) {
            m_context->Shutdown();
            m_context.reset();
        }
        SDL_Quit();
    }
}
