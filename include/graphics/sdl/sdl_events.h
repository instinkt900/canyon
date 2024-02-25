#pragma once

#include "events/event.h"
#include <SDL.h>

namespace graphics::sdl {
    std::unique_ptr<Event> FromSDL(SDL_Event const& event);
}

