#include "events/event_dispatch.h"
#include "graphics/vulkan/vulkan_subimage.h"
#include "platform/sdl/sdl_app.h"
#include "platform/glfw/glfw_app.h"
#include "layers/layer.h"
#include "graphics/sdl/sdl_graphics.h"
#include "graphics/vulkan/vulkan_graphics.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class TestLayer : public Layer {
public:
    TestLayer(graphics::IGraphics* graphics)
        : m_graphics(graphics) {
    }

    void Draw() override {
        m_graphics->SetColor(graphics::Color(1.0f, 1.0f, 1.0f, 1.0f));
        m_graphics->Clear();

        m_graphics->SetColor(graphics::Color(1.0f, 0, 0, 1.0f));
        m_graphics->DrawFillRectF(MakeRect(10.0f,10.0f,50.0f,50.0f));
    }

    graphics::IGraphics* m_graphics;
};

class VulkApplication : public GLFWApplication {
public:
    VulkApplication()
    :GLFWApplication("test") {
    }

    bool CreateWindow() override {
        if (!GLFWApplication::CreateWindow()) {
            return false;
        }
        m_context = std::make_unique<graphics::vulkan::Context>();
        glfwCreateWindowSurface(m_context->m_vkInstance, GetGLFWWindow(), nullptr, &m_customVkSurface);
        m_graphics = std::make_unique<graphics::vulkan::Graphics>(*m_context, m_customVkSurface, GetWidth(), GetHeight());
        return true;
    }

    bool OnEvent(Event const& event) override {
        if (!GLFWApplication::OnEvent(event)) {
            return false;
        }

        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &VulkApplication::OnWindowSizeEvent);
        dispatch.Dispatch(m_layerStack.get());
        return dispatch.GetHandled();
    }

    bool OnWindowSizeEvent(EventWindowSize const& event) {
        m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
        return true;
    }

    void Update() override {
        auto const nowTicks = std::chrono::steady_clock::now();
        auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
        while (deltaTicks > m_updateTicks) {
            GLFWApplication::Update();
            m_layerStack->Update(static_cast<uint32_t>(m_updateTicks.count()));
            m_lastUpdateTicks += m_updateTicks;
            deltaTicks -= m_updateTicks;
        }
    }

    void Draw() override {
        m_layerStack->Draw();
        GLFWApplication::Draw();
    }

private:
    std::unique_ptr<graphics::vulkan::Context> m_context;
    VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;
    std::unique_ptr<graphics::vulkan::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
    std::chrono::milliseconds m_updateTicks;
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
};

class TestApplication : public SDLApplication {
public:
    TestApplication()
        : SDLApplication("Testing") {
        m_updateTicks = std::chrono::milliseconds(1000 / 60);
    }

    bool CreateWindow() override {
        if (!SDLApplication::CreateWindow()) {
            return false;
        }

        m_graphics = std::make_unique<graphics::sdl::Graphics>(GetSDLRenderer());
        m_layerStack = std::make_unique<LayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
        m_layerStack->SetEventListener(this);

        std::unique_ptr<TestLayer> layer(std::make_unique<TestLayer>(m_graphics.get()));
        m_layerStack->PushLayer(std::move(layer));

        m_lastUpdateTicks = std::chrono::steady_clock::now();

        return true;
    }

    bool OnEvent(Event const& event) override {
        if (!SDLApplication::OnEvent(event)) {
            return false;
        }

        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &TestApplication::OnWindowSizeEvent);
        dispatch.Dispatch(m_layerStack.get());
        return dispatch.GetHandled();
    }

    bool OnWindowSizeEvent(EventWindowSize const& event) {
        m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
        return true;
    }

    void Update() override {
        auto const nowTicks = std::chrono::steady_clock::now();
        auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
        while (deltaTicks > m_updateTicks) {
            SDLApplication::Update();
            m_layerStack->Update(static_cast<uint32_t>(m_updateTicks.count()));
            m_lastUpdateTicks += m_updateTicks;
            deltaTicks -= m_updateTicks;
        }
    }

    void Draw() override {
        m_layerStack->Draw();
        SDLApplication::Draw();
    }

    std::unique_ptr<graphics::sdl::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
    std::chrono::milliseconds m_updateTicks;
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
};

int main(int argc, char* argv[]) {
    VulkApplication app{};
    // GLFWApplication app{"Testing"};
    app.Run();
    return 0;
}
