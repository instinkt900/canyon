#pragma once

#include "events/event.h"

std::unique_ptr<Event> FromSDL(SDL_Event const& event);

