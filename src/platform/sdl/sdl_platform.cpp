#include "canyon.h"
#include "platform/sdl/sdl_window.h"
#include "platform/sdl/sdl_platform.h"

namespace platform::sdl {
    std::unique_ptr<platform::Window> Platform::CreateWindow(char const* title, int width, int height) {
        return std::make_unique<platform::sdl::Window>("testing", 640, 480);
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

