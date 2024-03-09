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

    bool Window::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &Window::OnRequestQuitEvent);
        dispatch.Dispatch(this, &Window::OnQuitEvent);
        return dispatch.GetHandled();
    }

    bool Window::OnRequestQuitEvent(EventRequestQuit const& event) {
        return true;
    }

    bool Window::OnQuitEvent(EventQuit const& event) {
        return true;
    }
}
