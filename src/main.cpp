#include "canyon.h"
#include "layers/layer.h"
#include "platform/vulkan_application.h"
#include "platform/sdl_application.h"
#include "moth_ui/group.h"
#include "moth_ui/events/event.h"
#include "events/event_window.h"
#include "moth_ui/events/event_animation.h"
#include "ui_button.h"

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
    std::shared_ptr<moth_ui::Group> m_root;
    moth_ui::IntVec2 m_lastDrawnSize;
};

int main(int argc, char* argv[]) {
    // VulkApplication app{};
    SDLApplication app{};
    app.GetLayerStack().PushLayer(std::make_unique<TestLayer>(app.GetGraphics(), ""));
    app.TickSync();
    return 0;
}
