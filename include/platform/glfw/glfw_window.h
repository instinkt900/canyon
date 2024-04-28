#pragma once

#include "platform/window.h"

namespace platform::glfw {
    class Window : public platform::Window {
    public:
        Window(std::string const& title, int width, int height);
        virtual ~Window();

        void SetWindowTitle(std::string const& title) override;
        GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }

        void Update() override;

    protected:
        bool CreateWindow() override;
        void DestroyWindow() override;

    private:
        GLFWwindow* m_glfwWindow = nullptr;
        moth_ui::FloatVec2 m_lastMousePos;
        bool m_haveMousePos = false;

        void OnResize();
    };
}

