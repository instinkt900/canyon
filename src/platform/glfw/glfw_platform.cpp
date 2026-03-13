#include "common.h"
#include "canyon/platform/glfw/glfw_platform.h"
#include "canyon/platform/glfw/glfw_window.h"

namespace canyon::platform::glfw {
    bool Platform::Startup() {
        if (!glfwInit()) {
            spdlog::error("GLFW: initialization failed");
            return false;
        }
        spdlog::info("GLFW: initialized");
        m_context = std::make_unique<graphics::vulkan::Context>();
        return true;
    }

    void Platform::Shutdown() {
        spdlog::info("GLFW: shutting down");
        glfwTerminate();
    }
    
    graphics::Context& Platform::GetGraphicsContext() {
        return *m_context;
    }

    std::unique_ptr<canyon::platform::Window> Platform::CreateWindow(std::string const& title, int width, int height) {
        return std::make_unique<platform::glfw::Window>(*m_context, title, width, height);
    }
}


