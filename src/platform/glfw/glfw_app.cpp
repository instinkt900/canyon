#include "platform/glfw/glfw_app.h"
#include "platform/glfw/glfw_events.h"
#include "events/event_mouse.h"

GLFWApplication::GLFWApplication(std::string const& applicationTitle)
    : Application(applicationTitle) {
}

GLFWApplication::~GLFWApplication() {
}

void GLFWApplication::Update() {
    glfwPollEvents();

    if (glfwWindowShouldClose(m_glfwWindow)) {
        glfwSetWindowShouldClose(m_glfwWindow, 0);
        OnEvent(EventRequestQuit());
    }

    m_windowMaximized = glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
}

bool GLFWApplication::CreateWindow() {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_glfwWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, m_applicationTitle.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_glfwWindow, this);

    if (m_windowPos.x != -1 && m_windowPos.y != -1) {
        glfwSetWindowPos(m_glfwWindow, m_windowPos.x, m_windowPos.y);
    }

    glfwSetWindowPosCallback(m_glfwWindow, [](GLFWwindow* window, int xpos, int ypos) {
        GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
        app->m_windowMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
        if (!app->m_windowMaximized) {
            app->m_windowPos.x = xpos;
            app->m_windowPos.y = ypos;
        }
    });

    glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) {
        GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
        app->m_windowMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
        if (!app->m_windowMaximized) {
            app->m_windowWidth = width;
            app->m_windowHeight = height;
        }
        app->OnResize();
        const auto translatedEvent = std::make_unique<EventWindowSize>(width, height);
        app->OnEvent(*translatedEvent);
    });

    glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
        if (auto const translatedEvent = FromGLFW(key, scancode, action, mods)) {
            app->OnEvent(*translatedEvent);
        }
    });

    glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
        GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
        auto const newMousePos = FloatVec2{ xpos, ypos };
        FloatVec2 mouseDelta{ 0, 0 };
        if (app->m_haveMousePos) {
            mouseDelta = newMousePos - app->m_lastMousePos;
        }
        app->m_lastMousePos = newMousePos;
        app->m_haveMousePos = true;
        auto const translatedEvent = std::make_unique<EventMouseMove>(static_cast<IntVec2>(app->m_lastMousePos), static_cast<FloatVec2>(mouseDelta));
        app->OnEvent(*translatedEvent);
    });

    glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
        GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
        if (auto const translatedEvent = FromGLFW(button, action, mods, static_cast<IntVec2>(app->m_lastMousePos))) {
            app->OnEvent(*translatedEvent);
        }
    });

    int width, height;
    glfwGetFramebufferSize(m_glfwWindow, &width, &height);

    if (m_windowMaximized) {
        glfwMaximizeWindow(m_glfwWindow);
    }

    return true;
}

void GLFWApplication::SetWindowTitle(std::string const& title) {
    m_applicationTitle = title;
    glfwSetWindowTitle(m_glfwWindow, m_applicationTitle.c_str());
}

void GLFWApplication::Draw() {
}

void GLFWApplication::DestroyWindow() {
    m_windowMaximized = glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void GLFWApplication::OnResize() {

}
