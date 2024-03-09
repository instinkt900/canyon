#include "platform_window.h"
#include "events/event_dispatch.h"

namespace platform {
    Window::Window(std::string const& title, int width, int height)
        : m_title(title)
        , m_windowWidth(width)
        , m_windowHeight(height) {
    }

    Window::~Window() {
    }
}
