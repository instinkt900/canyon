#pragma once

#include <moth_ui/events/event.h>
#include <moth_ui/utils/vector.h>

#include <memory>

namespace canyon::platform::glfw {
    std::unique_ptr<moth_ui::Event> FromGLFW(int key, int scancode, int action, int mods);
    std::unique_ptr<moth_ui::Event> FromGLFW(int button, int action, int mods, moth_ui::IntVec2 const& pos);
}
