#include "moth_graphics/graphics/vulkan/vulkan_utils.h"
#include "common.h"
#include "moth_graphics/platform/glfw/glfw_window.h"
#include "moth_graphics/graphics/vulkan/vulkan_graphics.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "moth_graphics/platform/glfw/glfw_events.h"
#include "moth_graphics/events/event_window.h"
#include "moth_graphics/graphics/moth_ui/utils.h"
#include <moth_ui/events/event_mouse.h>

namespace moth_graphics::platform::glfw {
    Window::Window(graphics::vulkan::Context& context, std::string_view title, int width, int height)
        : moth_graphics::platform::Window(title, width, height)
        , m_context(context) {
        if (CreateWindow()) {
            PostCreate();
        }
    }

    Window::~Window() {
        DestroyWindow();
    }

    void Window::Update(uint32_t ticks) {
        glfwPollEvents();

        if (glfwWindowShouldClose(m_glfwWindow) != 0) {
            glfwSetWindowShouldClose(m_glfwWindow, 0);
            EmitEvent(EventRequestQuit());
        }

        m_windowMaximized = glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
        m_layerStack->Update(ticks);
    }

    void Window::Draw() {
        if (!m_graphics->Begin()) {
            // Swapchain is out of date (e.g. minimised or mid-resize).
            // Skip rendering this frame; Begin() has already triggered recreation
            // if the surface extent was non-zero.
            return;
        }
        m_layerStack->Draw();
        m_graphics->End();
    }

    bool Window::CreateWindow() {
        spdlog::info("GLFW: creating window '{}' ({}x{})", m_title, m_windowWidth, m_windowHeight);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_glfwWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, m_title.c_str(), nullptr, nullptr);
        if (m_glfwWindow == nullptr) {
            spdlog::error("GLFW: failed to create window '{}'", m_title);
            return false;
        }
        glfwSetWindowUserPointer(m_glfwWindow, this);

        if (m_windowPos.x != -1 && m_windowPos.y != -1) {
            glfwSetWindowPos(m_glfwWindow, m_windowPos.x, m_windowPos.y);
        }

        glfwSetWindowPosCallback(m_glfwWindow, [](GLFWwindow* window, int xpos, int ypos) {
            Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app == nullptr) {
                return;
            }
            app->m_windowMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
            if (!app->m_windowMaximized) {
                app->m_windowPos.x = xpos;
                app->m_windowPos.y = ypos;
            }
        });

        glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* window, int width, int height) {
            Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app == nullptr) {
                return;
            }
            app->m_windowMaximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
            if (!app->m_windowMaximized) {
                app->m_windowWidth = width;
                app->m_windowHeight = height;
            }
            app->OnResize();
            const auto translatedEvent = std::make_unique<EventWindowSize>(width, height);
            app->EmitEvent(*translatedEvent);
        });

        glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app == nullptr) {
                return;
            }
            if (auto const translatedEvent = FromGLFW(key, scancode, action, mods)) {
                app->EmitEvent(*translatedEvent);
            }
        });

        glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
            Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app == nullptr) {
                return;
            }
            auto const newMousePos = FloatVec2{ xpos, ypos };
            FloatVec2 mouseDelta{ 0, 0 };
            if (app->m_haveMousePos) {
                mouseDelta = newMousePos - app->m_lastMousePos;
            }
            app->m_lastMousePos = newMousePos;
            app->m_haveMousePos = true;
            auto lastMousePos = static_cast<moth_ui::IntVec2>(app->m_lastMousePos);
            auto const translatedEvent = std::make_unique<moth_ui::EventMouseMove>(lastMousePos, mouseDelta);
            app->EmitEvent(*translatedEvent);
        });

        glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
            Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app == nullptr) {
                return;
            }
            if (auto const translatedEvent = FromGLFW(button, action, mods, static_cast<moth_ui::IntVec2>(app->m_lastMousePos))) {
                app->EmitEvent(*translatedEvent);
            }
        });

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_glfwWindow, &width, &height);

        if (m_windowMaximized) {
            glfwMaximizeWindow(m_glfwWindow);
        }

        CHECK_VK_RESULT(glfwCreateWindowSurface(m_context.GetInstance(), m_glfwWindow, nullptr, &m_customVkSurface));
        m_surfaceContext = std::make_unique<graphics::vulkan::SurfaceContext>(m_context);

        {
            float xscale = 1.0f;
            float yscale = 1.0f;
            glfwGetWindowContentScale(m_glfwWindow, &xscale, &yscale);
            // TODO: this scaling factor was derived from comparing sdl and vulkan fonts pixel sizes
            m_surfaceContext->SetDPI(static_cast<int>(xscale * 74.0f));
        }

        m_graphics = std::make_unique<graphics::vulkan::Graphics>(*m_surfaceContext, m_customVkSurface, m_windowWidth, m_windowHeight);
        spdlog::info("GLFW: window '{}' ready", m_title);
        return true;
    }

    void Window::SetWindowTitle(std::string_view title) {
        m_title = title;
        glfwSetWindowTitle(m_glfwWindow, m_title.c_str());
    }

    void Window::DestroyWindow() {
        spdlog::info("GLFW: destroying window '{}'", m_title);
        // Wait for all GPU work to finish before destroying Vulkan-backed resources
        if (m_surfaceContext) {
            vkDeviceWaitIdle(m_surfaceContext->GetVkDevice());
        }
        PreDestroy();
        // TODO: why do we do this in destroy?
        m_windowMaximized = glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
        m_graphics = nullptr;
        m_surfaceContext = nullptr;
        if (m_customVkSurface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(m_context.GetInstance(), m_customVkSurface, nullptr);
            m_customVkSurface = VK_NULL_HANDLE;
        }
        glfwDestroyWindow(m_glfwWindow);
    }

    void Window::OnResize() {
        int fbWidth = 0;
        int fbHeight = 0;
        glfwGetFramebufferSize(m_glfwWindow, &fbWidth, &fbHeight);
        spdlog::info("GLFW: window '{}' resized to {}x{} (framebuffer {}x{})",
                     m_title, m_windowWidth, m_windowHeight, fbWidth, fbHeight);
        if (fbWidth == 0 || fbHeight == 0) {
            // Window is minimised or has a zero dimension; skip swapchain recreation.
            return;
        }
        auto* graphics = dynamic_cast<graphics::vulkan::Graphics*>(m_graphics.get());
        if (graphics == nullptr) {
            spdlog::error("GLFW: OnResize called but graphics backend is not Vulkan");
            return;
        }
        graphics->OnResize(m_customVkSurface, static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight));
        m_layerStack->SetWindowSize({ m_windowWidth, m_windowHeight });
    }
}
