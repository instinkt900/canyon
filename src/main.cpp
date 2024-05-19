#include "canyon.h"
#include "graphics/sdl/sdl_font.h"
#include "graphics/sdl/sdl_font_factory.h"
#include "graphics/sdl/sdl_image_factory.h"
#include "graphics/sdl/sdl_ui_renderer.h"
#include "layers/layer.h"
#include "platform/vulkan_application.h"
#include "platform/sdl_application.h"
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

class TestLayer : public Layer {
public:
    TestLayer(graphics::IGraphics& graphics, std::filesystem::path const& layoutPath)
        : m_graphics(graphics) {
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
        m_root = moth_ui::NodeFactory::Get().Create(path, GetWidth(), GetHeight());
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

    graphics::IGraphics& m_graphics;
    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::FontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;

    std::shared_ptr<moth_ui::Group> m_root;
    moth_ui::IntVec2 m_lastDrawnSize;
};

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        return false;
    }

    graphics::vulkan::Context context;
    auto window = std::make_unique<platform::glfw::Window>(context, "testing", 640, 480);
    auto graphics = std::make_unique<graphics::vulkan::Graphics>(context, window->GetVkSurface(), window->GetWidth(), window->GetHeight());
    auto texture = graphics::vulkan::Image::Load(context, "assets/images/playership.png");
    auto font = graphics::vulkan::Font::Load("assets/fonts/pilotcommand.ttf", 12, context, *graphics);

    while (true) {
        graphics->Begin();
        graphics->SetColor(graphics::BasicColors::Red);
        graphics->DrawFillRectF(MakeRect(0.0f, 0.0f, 100.0f, 100.0f));
        graphics->SetColor(graphics::BasicColors::White);
        auto destRect = MakeRect(100, 100, 200, 200);
        graphics->DrawImage(*texture, destRect, nullptr);
        graphics->SetColor(graphics::BasicColors::Green);
        graphics->DrawText("hello", *font, graphics::TextHorizAlignment::Center, graphics::TextVertAlignment::Middle, MakeRect(0, 0, 640, 480));
        window->Draw();
        graphics->End();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }

    // // VulkApplication app{};
    // SDLApplication app{};
    // moth_ui::Context::GetCurrentContext()->GetFontFactory().LoadProject("assets/fonts.json");
    // app.GetWindow().GetLayerStack().PushLayer(std::make_unique<TestLayer>(app.GetWindow().GetGraphics(), "assets/layouts/basic.mothui"));
    // app.TickSync();
    return 0;
}
