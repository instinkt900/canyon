#pragma once

#include <moth_ui/events/event.h>
#include <SDL_events.h>

#include <memory>

namespace canyon::platform::sdl {
    std::unique_ptr<moth_ui::Event> FromSDL(SDL_Event const& event);
}
