#pragma once

#include "events/event.h"
#include <SDL.h>

std::unique_ptr<Event> FromSDL(SDL_Event const& event);

