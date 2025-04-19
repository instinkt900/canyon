#include "example_layer.h"
#include "imgui.h"
#include "ui_button.h"

#include <moth_ui/group.h>
#include <moth_ui/event_dispatch.h>

TestLayer::TestLayer(moth_ui::Context& context, canyon::graphics::IGraphics& graphics, std::filesystem::path const& layoutPath)
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

TestLayer::~TestLayer() {
}

bool TestLayer::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    // dispatch.Dispatch(this, &TestLayer::OnQuitEvent);
    bool handled = dispatch.GetHandled();
    if (!handled && m_root) {
        handled = m_root->SendEvent(event, moth_ui::Node::EventDirection::Down);
    }
    return handled;
}

void TestLayer::Update(uint32_t ticks) {
    if (m_root) {
        m_root->Update(ticks);
    }
}

void TestLayer::Draw() {
    ImGui::ShowDemoWindow();
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

void TestLayer::OnAddedToStack(moth_ui::LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    if (m_root) {
        moth_ui::IntRect rect;
        rect.topLeft = { 0, 0 };
        rect.bottomRight = { GetWidth(), GetHeight() };
        m_root->SetScreenRect(rect);
    }
}

void TestLayer::LoadLayout(std::filesystem::path const& path) {
    m_root = moth_ui::NodeFactory::Get().Create(m_context, path, GetWidth(), GetHeight());
    m_root->SetEventHandler([this](moth_ui::Node*, moth_ui::Event const& event) { return OnUIEvent(event); });
}

bool TestLayer::OnUIEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &TestLayer::OnAnimationStopped);
    return dispatch.GetHandled();
}

bool TestLayer::OnAnimationStopped(moth_ui::EventAnimationStopped const& event) {
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

// bool TestLayer::OnQuitEvent(QuitEvent  const& event) {
//     m_layerStack->BroadcastEvent(EventQuit());
//     return true;
// }
