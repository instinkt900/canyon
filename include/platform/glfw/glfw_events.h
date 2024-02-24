#pragma once

#include "events/event.h"
#include "utils/vector.h"

std::unique_ptr<Event> FromGLFW(int key, int scancode, int action, int mods);
std::unique_ptr<Event> FromGLFW(int button, int action, int mods, IntVec2 const& pos);
