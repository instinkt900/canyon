#include "common.h"
#include "moth_graphics/platform/glfw/glfw_platform.h"
#include "moth_graphics/platform/glfw/glfw_window.h"

namespace moth_graphics::platform::glfw {
    bool Platform::Startup() {
        if (glfwInit() == 0) {
            spdlog::error("GLFW: initialization failed");
            return false;
        }
        spdlog::info("GLFW: initialized");
        m_context = std::make_unique<graphics::vulkan::Context>();
        if (!m_context->Startup()) {
            spdlog::error("GLFW: graphics context startup failed");
            m_context.reset();
            glfwTerminate();
            return false;
        }
        return true;
    }

    void Platform::Shutdown() {
        spdlog::info("GLFW: shutting down");
        if (m_context) {
            m_context->Shutdown();
            m_context.reset();
        }
        glfwTerminate();
    }
    
    graphics::Context& Platform::GetGraphicsContext() {
        return *m_context;
    }

    std::unique_ptr<moth_graphics::platform::Window> Platform::CreateWindow(std::string_view title, int width, int height) {
        return std::make_unique<platform::glfw::Window>(*m_context, title, width, height);
    }
}


