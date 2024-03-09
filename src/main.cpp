#include "events/event_dispatch.h"
#include "graphics/vulkan/vulkan_subimage.h"
#include "platform/sdl/sdl_window.h"
#include "platform/glfw/glfw_window.h"
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

class Ticker {
public:
    Ticker() {}
    virtual ~Ticker() {}

    void SetRunning(bool running) { m_running = false; }

    void Run() {
        m_running = true;
        while (m_running) {
            auto const nowTicks = std::chrono::steady_clock::now();
            auto deltaTicks = std::chrono::duration_cast<std::chrono::milliseconds>(nowTicks - m_lastUpdateTicks);
            while (deltaTicks > m_updateTicks) {
                Tick(static_cast<uint32_t>(m_updateTicks.count()));
                m_lastUpdateTicks += m_updateTicks;
                deltaTicks -= m_updateTicks;
            }
        }
    }

protected:
    virtual void Tick(uint32_t ticks) = 0;

private:
    bool m_running;
    std::chrono::milliseconds m_updateTicks;
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
};

class VulkApplication : public Ticker, public EventListener {
public:
    VulkApplication() {
        m_window = std::make_unique<platform::glfw::Window>("testing", 640, 480);
        m_window->AddEventListener(this);
        m_context = std::make_unique<graphics::vulkan::Context>();
        glfwCreateWindowSurface(m_context->m_vkInstance, m_window->GetGLFWWindow(), nullptr, &m_customVkSurface);
        m_graphics = std::make_unique<graphics::vulkan::Graphics>(*m_context, m_customVkSurface, m_window->GetWidth(), m_window->GetHeight());
        m_layerStack = std::make_unique<LayerStack>(m_window->GetWidth(), m_window->GetHeight(), m_window->GetWidth(), m_window->GetHeight());
        m_layerStack->AddEventListener(this);
        std::unique_ptr<TestLayer> layer(std::make_unique<TestLayer>(m_graphics.get()));
        m_layerStack->PushLayer(std::move(layer));
    }

    bool OnEvent(Event const& event) override {
        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &VulkApplication::OnWindowSizeEvent);
        dispatch.Dispatch(this, &VulkApplication::OnRequestQuitEvent);
        dispatch.Dispatch(this, &VulkApplication::OnQuitEvent);
        dispatch.Dispatch(m_layerStack.get());
        return dispatch.GetHandled();
    }

    bool OnWindowSizeEvent(EventWindowSize const& event) {
        m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
        return true;
    }

    bool OnRequestQuitEvent(EventRequestQuit const& event) {
        SetRunning(false);
        return true;
    }

    bool OnQuitEvent(EventQuit const& event) {
        SetRunning(false);
        return true;
    }

    void Tick(uint32_t ticks) override {
        m_window->Update();
        m_layerStack->Update(ticks);
        m_graphics->Begin();
        m_layerStack->Draw();
        m_window->Draw();
        m_graphics->End();
    }

private:
    std::unique_ptr<platform::glfw::Window> m_window;
    std::unique_ptr<graphics::vulkan::Context> m_context;
    VkSurfaceKHR m_customVkSurface = VK_NULL_HANDLE;
    std::unique_ptr<graphics::vulkan::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
};

class SDLApplication : public Ticker, public EventListener {
public:
    SDLApplication() {
        m_window = std::make_unique<platform::sdl::Window>("testing", 640, 480);
        m_window->AddEventListener(this);
        m_graphics = std::make_unique<graphics::sdl::Graphics>(m_window->GetSDLRenderer());
        m_layerStack = std::make_unique<LayerStack>(m_window->GetWidth(), m_window->GetHeight(), m_window->GetWidth(), m_window->GetHeight());
        m_layerStack->AddEventListener(this);
        std::unique_ptr<TestLayer> layer(std::make_unique<TestLayer>(m_graphics.get()));
        m_layerStack->PushLayer(std::move(layer));
    }

    bool OnEvent(Event const& event) override {
        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &SDLApplication::OnWindowSizeEvent);
        dispatch.Dispatch(m_layerStack.get());
        dispatch.Dispatch(this, &SDLApplication::OnRequestQuitEvent);
        dispatch.Dispatch(this, &SDLApplication::OnQuitEvent);
        return dispatch.GetHandled();
    }

    bool OnWindowSizeEvent(EventWindowSize const& event) {
        m_layerStack->SetWindowSize({ event.GetWidth(), event.GetHeight() });
        return true;
    }

    bool OnRequestQuitEvent(EventRequestQuit const& event) {
        SetRunning(false);
        return true;
    }

    bool OnQuitEvent(EventQuit const& event) {
        SetRunning(false);
        return true;
    }

    void Tick(uint32_t ticks) override {
        m_window->Update();
        m_layerStack->Update(ticks);
        m_layerStack->Draw();
        m_window->Draw();
    }

private:
    std::unique_ptr<platform::sdl::Window> m_window;
    std::unique_ptr<graphics::sdl::Graphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
};

int main(int argc, char* argv[]) {
    VulkApplication app{};
    //SDLApplication app{};
    app.Run();
    return 0;
}
