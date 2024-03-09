#pragma once

#include "application.h"
#include <GLFW/glfw3.h>

class GLFWApplication : public Application {
public:
    GLFWApplication(std::string const& applicationTitle);
    virtual ~GLFWApplication();

    void SetWindowTitle(std::string const& title) override;
    GLFWwindow* GetGLFWWindow() const { return m_glfwWindow; }

protected:
    bool CreateWindow() override;
    void DestroyWindow() override;
    void Update() override;
    void Draw() override;

private:
    GLFWwindow* m_glfwWindow = nullptr;
    FloatVec2 m_lastMousePos;
    bool m_haveMousePos = false;

    void OnResize();
};
