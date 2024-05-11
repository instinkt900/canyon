#pragma once

#include "graphics/vulkan/vulkan_context.h"
#include "platform/window.h"

namespace platform::glfw {
    class Window : public platform::Window {
    public:
        Window(graphics::vulkan::Context& context, std::string const& title, int width, int height);
        virtual ~Window();

        void SetWindowTitle(std::string const& title) override;
        GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        graphics::vulkan::Context& m_vulkanContext;
        GLFWwindow* m_glfwWindow = nullptr;
        FloatVec2 m_lastMousePos;
        bool m_haveMousePos = false;
        VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;

        void OnResize();
    };
}

