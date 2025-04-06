#pragma once

#include "graphics/vulkan/vulkan_context.h"
#include "graphics/vulkan/vulkan_surface_context.h"
#include "platform/window.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace platform::glfw {
    class Window : public platform::Window {
    public:
        Window(graphics::vulkan::Context& context, std::string const& title, int width, int height);
        virtual ~Window();

        graphics::SurfaceContext & GetSurfaceContext() const override { return *m_surfaceContext; }
        void SetWindowTitle(std::string const& title) override;
        GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }
        VkSurfaceKHR GetVkSurface() const { return m_customVkSurface; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        graphics::vulkan::Context& m_context;
        std::unique_ptr<graphics::vulkan::SurfaceContext> m_surfaceContext;

        GLFWwindow* m_glfwWindow = nullptr;
        FloatVec2 m_lastMousePos;
        bool m_haveMousePos = false;
        VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;

        void OnResize();
    };
}

