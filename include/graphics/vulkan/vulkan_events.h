#pragma once

#include "moth_ui/events/event.h"
#include "utils/vector.h"

namespace graphics::vulkan {
    std::unique_ptr<moth_ui::Event> FromGLFW(int key, int scancode, int action, int mods);
    std::unique_ptr<moth_ui::Event> FromGLFW(int button, int action, int mods, IntVec2 const& pos);
}
