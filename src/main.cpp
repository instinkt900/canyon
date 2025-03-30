#include "canyon.h"
#include "graphics/sdl/sdl_font.h"
#include "layers/layer.h"
#include "platform/glfw/glfw_platform.h"
#include "platform/sdl/sdl_platform.h"
#include "platform/glfw/glfw_platform.h"
#include "platform/application.h"
#include "moth_ui/group.h"
#include "moth_ui/events/event.h"
#include "events/event_window.h"
#include "moth_ui/events/event_animation.h"
#include "ui_button.h"
#include "moth_ui/iimage_factory.h"
#include "moth_ui/font_factory.h"
#include "moth_ui/node_factory.h"
#include "moth_ui/context.h"
#include "graphics/sdl/sdl_image.h"
#include "graphics/sdl/sdl_graphics.h"
#include "graphics/vulkan/vulkan_image.h"
#include "graphics/vulkan/vulkan_font.h"
#include <moth_ui/event_listener.h>

class TestLayer : public Layer {
public:
    TestLayer(moth_ui::Context& context, graphics::IGraphics& graphics, std::filesystem::path const& layoutPath)
        : m_context(context)
        , m_graphics(graphics) {
        LoadLayout(layoutPath);

        m_root->SetAnimation("ready");
        if (auto startButton = m_root->FindChild<UIButton>("button")) {
            startButton->SetClickAction([&]() {
                m_root->SetAnimation("transition_out");
            });
        }
    }

    bool OnEvent(moth_ui::Event const& event) override {
        moth_ui::EventDispatch dispatch(event);
        // dispatch.Dispatch(this, &TestLayer::OnQuitEvent);
        bool handled = dispatch.GetHandled();
        if (!handled && m_root) {
            handled = m_root->SendEvent(event, moth_ui::Node::EventDirection::Down);
        }
        return handled;
    }

    void Update(uint32_t ticks) override {
        if (m_root) {
            m_root->Update(ticks);
        }
    }

    void Draw() override {
        moth_ui::IntVec2 const currentSize{ GetWidth(), GetHeight() };
        if (m_lastDrawnSize != currentSize) {
            moth_ui::IntRect displayRect;
            displayRect.topLeft = { 0, 0 };
            displayRect.bottomRight = currentSize;
            m_root->SetScreenRect(displayRect);
        }
        if (m_root) {
            m_root->Draw();
        }
        m_lastDrawnSize = currentSize;
    }

    void OnAddedToStack(LayerStack* stack) override {
        Layer::OnAddedToStack(stack);

        if (m_root) {
            moth_ui::IntRect rect;
            rect.topLeft = { 0, 0 };
            rect.bottomRight = { GetWidth(), GetHeight() };
            m_root->SetScreenRect(rect);
        }
    }

    void LoadLayout(std::filesystem::path const& path) {
        m_root = moth_ui::NodeFactory::Get().Create(m_context, path, GetWidth(), GetHeight());
        m_root->SetEventHandler([this](moth_ui::Node*, moth_ui::Event const& event) { return OnUIEvent(event); });
    }

    bool OnUIEvent(moth_ui::Event const& event) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch(this, &TestLayer::OnAnimationStopped);
        return dispatch.GetHandled();
    }

    bool OnAnimationStopped(moth_ui::EventAnimationStopped const& event) {
        if (event.GetClipName() == "ready") {
            m_root->SetAnimation("idle");
            return true;
        } else if (event.GetClipName() == "transition_out") {
            LoadLayout("assets/layouts/demo.mothui");
            m_root->SetAnimation("transition_in");
            return true;
        }
        return false;
    }

    // bool OnQuitEvent(QuitEvent  const& event) {
    //     m_layerStack->BroadcastEvent(EventQuit());
    //     return true;
    // }

    moth_ui::Context& m_context;
    graphics::IGraphics& m_graphics;
    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::FontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;

    std::shared_ptr<moth_ui::Group> m_root;
    moth_ui::IntVec2 m_lastDrawnSize;
};

class QuitListener : public moth_ui::EventListener {
public:
    QuitListener(bool& signal)
        : m_signal(signal) {}
    bool OnEvent(moth_ui::Event const& event) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch<EventRequestQuit>([&](EventRequestQuit const& event) {
            m_signal = false;
            return true;
        });
        return dispatch.GetHandled();
    }

private:
    bool& m_signal;
};

int main(int argc, char* argv[]) {
    // auto platform = std::make_unique<platform::sdl::Platform>();
    auto platform = std::make_unique<platform::glfw::Platform>();
    platform->Startup();
    auto application = std::make_unique<Application>(*platform);
    auto& window = application->GetWindow();
    auto& mothContext = window.GetMothContext();
    mothContext.GetFontFactory().LoadProject("assets/fonts.json");
    auto& graphics = window.GetGraphics();
    window.GetLayerStack().PushLayer(std::make_unique<TestLayer>(mothContext, graphics, "assets/layouts/basic.mothui"));
    application->TickSync();

    // auto window = platform->CreateWindow("testing", 640, 480);
    // auto& surfaceContext = window->GetSurfaceContext();
    // auto texture = surfaceContext.ImageFromFile("assets/images/playership.png");
    // auto font = surfaceContext.FontFromFile("assets/fonts/pilotcommand.ttf", 38);
    //
    // bool running = true;
    // QuitListener ql(running);
    // window->AddEventListener(&ql);
    //
    // auto& graphics = window->GetGraphics();
    // while (running) {
    //     graphics.Begin();
    //     graphics.SetColor(graphics::BasicColors::Red);
    //     graphics.DrawFillRectF(MakeRect(0.0f, 0.0f, 100.0f, 100.0f));
    //     graphics.SetColor(graphics::BasicColors::White);
    //     auto destRect = MakeRect(0, 0, 640, 480);
    //     graphics.DrawImageTiled(*texture, destRect, nullptr, 1.0f);
    //     graphics.SetBlendMode(graphics::BlendMode::Alpha);
    //     graphics.SetColor(graphics::BasicColors::Green);
    //     graphics.DrawText("hello", *font, graphics::TextHorizAlignment::Center, graphics::TextVertAlignment::Middle, MakeRect(0, 0, 640, 480));
    //     window->Update(30);
    //     // window->Draw();
    //     graphics.End();
    //     using namespace std::chrono_literals;
    //     std::this_thread::sleep_for(1ms);
    // }

    platform->Shutdown();
    return 0;
}
