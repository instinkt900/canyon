#pragma once

#include "moth_graphics/graphics/vulkan/vulkan_context.h"
#include "moth_graphics/graphics/vulkan/vulkan_surface_context.h"
#include "moth_graphics/graphics/surface_context.h"
#include "moth_graphics/platform/window.h"
#include "moth_graphics/utils/vector.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <string>
#include <cstdint>

namespace moth_graphics::platform::glfw {
    class Window : public moth_graphics::platform::Window {
    public:
        Window(graphics::vulkan::Context& context, std::string const& title, int width, int height);
        ~Window() override;

        graphics::SurfaceContext & GetSurfaceContext() const override { return *m_surfaceContext; }
        void SetWindowTitle(std::string const& title) override;
        GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }
        VkSurfaceKHR GetVkSurface() const { return m_customVkSurface; }

        void Update(uint32_t ticks) override;
        void Draw() override;

    private:
        bool CreateWindow();
        void DestroyWindow();


        graphics::vulkan::Context& m_context;
        std::unique_ptr<graphics::vulkan::SurfaceContext> m_surfaceContext;

        GLFWwindow* m_glfwWindow = nullptr;
        FloatVec2 m_lastMousePos;
        bool m_haveMousePos = false;
        VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;

        void OnResize();
    };
}

