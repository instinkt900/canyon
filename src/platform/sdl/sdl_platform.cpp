#include "canyon.h"
#include "graphics/sdl/sdl_context.h"
#include "graphics/sdl/sdl_graphics.h"
#include "platform/sdl/sdl_window.h"
#include "platform/sdl/sdl_platform.h"

namespace platform::sdl {
    graphics::Context& Platform::GetGraphicsContext() {
        // lazy init this will mean the first window created is linked to this context
        if (!m_context) {
            m_context = std::make_unique<graphics::sdl::Context>();
        }
        return *m_context;
    }

    graphics::IGraphics& Platform::GetGraphics() {
        if (!m_graphics) {
            m_graphics = std::make_unique<graphics::sdl::Graphics>(*m_context);
        }
        return *m_graphics;
    }

    std::unique_ptr<platform::Window> Platform::CreateWindow(char const* title, int width, int height) {
        return std::make_unique<platform::sdl::Window>(*this, title, width, height);
    }

    bool Platform::Startup() {
        if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
            return false;
        }

        return true;
    }

    void Platform::Shutdown() {
        SDL_Quit();
    }
}
