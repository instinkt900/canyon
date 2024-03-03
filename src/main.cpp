#include "events/event_dispatch.h"
#include "platform/sdl/sdl_app.h"
#include "platform/glfw/glfw_app.h"
#include "layers/layer.h"
#include "graphics/sdl/sdl_graphics.h"

using namespace graphics::sdl;

class TestLayer : public Layer {
public:
    TestLayer(SDLGraphics* graphics)
        : m_graphics(graphics) {
    }

    void Draw() override {
        m_graphics->SetColor(graphics::Color(1.0f, 1.0f, 1.0f, 1.0f));
        m_graphics->Clear();

        m_graphics->SetColor(graphics::Color(1.0f, 0, 0, 1.0f));
        m_graphics->DrawFillRectF(MakeRect(10.0f,10.0f,50.0f,50.0f));
    }

    graphics::sdl::SDLGraphics* m_graphics;
};

class BaseApplication : public GLFWApplication {
    public:
        BaseApplication(std::string const& title)
            :GLFWApplication(title) {}
};

class TestApplication : public BaseApplication {
public:
    TestApplication()
        : BaseApplication("Testing") {
        m_updateTicks = std::chrono::milliseconds(1000 / 60);
    }

    bool CreateWindow() override {
        if (!BaseApplication::CreateWindow()) {
            return false;
        }

        m_graphics = std::make_unique<SDLGraphics>(GetSDLRenderer());
        m_layerStack = std::make_unique<LayerStack>(m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
        m_layerStack->SetEventListener(this);

        std::unique_ptr<TestLayer> layer(std::make_unique<TestLayer>(m_graphics.get()));
        m_layerStack->PushLayer(std::move(layer));

        m_lastUpdateTicks = std::chrono::steady_clock::now();

        return true;
    }

    bool OnEvent(Event const& event) override {
        if (!BaseApplication::OnEvent(event)) {
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
            BaseApplication::Update();
            m_layerStack->Update(static_cast<uint32_t>(m_updateTicks.count()));
            m_lastUpdateTicks += m_updateTicks;
            deltaTicks -= m_updateTicks;
        }
    }

    void Draw() override {
        m_layerStack->Draw();
        BaseApplication::Draw();
    }

    std::unique_ptr<SDLGraphics> m_graphics;
    std::unique_ptr<LayerStack> m_layerStack;
    std::chrono::milliseconds m_updateTicks;
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdateTicks;
};

int main(int argc, char* argv[]) {
    TestApplication app{};
    // GLFWApplication app{"Testing"};
    app.Run();
    return 0;
}
