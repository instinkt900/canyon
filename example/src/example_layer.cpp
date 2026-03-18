#include "example_layer.h"
#include "imgui.h"
#include "ui_button.h"

#include <moth_ui/nodes/group.h>
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/layers/layer_stack.h>
#include "canyon/events/event_window.h"

#include <cmath>

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

    // Render target large enough to show off off-screen drawing
    m_target = graphics.CreateTarget(256, 256);
}

TestLayer::~TestLayer() {
}

bool TestLayer::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &TestLayer::OnKey);
    bool handled = dispatch.GetHandled();
    if (!handled && m_root) {
        handled = m_root->SendEvent(event, moth_ui::Node::EventDirection::Down);
    }
    return handled;
}

void TestLayer::Update(uint32_t ticks) {
    m_elapsedMs += ticks;

    if (m_root) {
        m_root->Update(ticks);
    }
}

void TestLayer::Draw() {
    // Info overlay showing what each part of the demo illustrates
    ImGui::Begin("Canyon Feature Demo", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Top-right  Render target (animated, 256x256)");
    ImGui::Text("           DrawLineF + DrawFillRectF");
    ImGui::Text("           Additive and alpha blend modes");
    ImGui::Text("Bottom-right  Clip rect demo");
    ImGui::Text("Centre     moth_ui layout + animations");
    ImGui::Text("           Custom UIButton widget");
    ImGui::Separator();
    ImGui::Text("S  save screenshot.png");
    ImGui::Separator();
    if (ImGui::Button("Save screenshot")) {
        m_pendingScreenshot = true;
    }
    ImGui::End();

    // Render an animated scene into the off-screen target each frame.
    // This demonstrates: render targets, primitive drawing, blend modes.
    bool const targetValid = m_target != nullptr &&
                             m_target->GetImage() != nullptr &&
                             m_target->GetImage()->GetWidth() > 0 &&
                             m_target->GetImage()->GetHeight() > 0;
    if (targetValid) {
        m_graphics.SetTarget(m_target.get());

        m_graphics.SetBlendMode(canyon::graphics::BlendMode::Replace);
        m_graphics.SetColor({ 0.05f, 0.05f, 0.15f, 1.0f });
        m_graphics.Clear();

        // Eight spinning coloured lines radiating from the centre
        float const cx = 128.0f, cy = 128.0f, radius = 110.0f;
        float const angle = m_elapsedMs * 0.001f;
        m_graphics.SetBlendMode(canyon::graphics::BlendMode::Add);
        for (int i = 0; i < 8; i++) {
            float t = angle + i * (3.14159f / 4.0f);
            canyon::FloatVec2 p0 = { cx + radius * std::cos(t), cy + radius * std::sin(t) };
            canyon::FloatVec2 p1 = { cx - radius * std::cos(t), cy - radius * std::sin(t) };
            float r = std::sin(t + angle) * 0.5f + 0.5f;
            float g = std::cos(t * 1.3f) * 0.5f + 0.5f;
            m_graphics.SetColor({ r, g, 1.0f - r, 1.0f });
            m_graphics.DrawLineF(p0, p1);
        }

        // Pulsing filled rect in the centre (demonstrates alpha blending)
        float pulse = std::sin(m_elapsedMs * 0.003f) * 0.5f + 0.5f;
        float half = 20.0f + 20.0f * pulse;
        m_graphics.SetBlendMode(canyon::graphics::BlendMode::Alpha);
        m_graphics.SetColor({ 1.0f, 0.5f + 0.5f * pulse, 0.2f, 0.75f });
        m_graphics.DrawFillRectF({ { cx - half, cy - half }, { cx + half, cy + half } });
        m_graphics.SetBlendMode(canyon::graphics::BlendMode::Replace);
        m_graphics.SetColor(canyon::graphics::BasicColors::White);
        m_graphics.DrawRectF({ { cx - half, cy - half }, { cx + half, cy + half } });

        m_graphics.SetTarget(nullptr);
    }

    // Recreate the full-screen target whenever the window is resized.
    moth_ui::IntVec2 const currentSize{ GetWidth(), GetHeight() };
    if (!m_screenTarget || m_lastDrawnSize != currentSize) {
        if (currentSize.x > 0 && currentSize.y > 0) {
            m_screenTarget = m_graphics.CreateTarget(currentSize.x, currentSize.y);
            if (m_root) {
                moth_ui::IntRect displayRect;
                displayRect.topLeft = { 0, 0 };
                displayRect.bottomRight = currentSize;
                m_root->SetScreenRect(displayRect);
            }
            m_lastDrawnSize = currentSize;
        }
    }

    if (!m_screenTarget) {
        return;
    }

    // Draw the whole scene into the full-screen target.
    m_graphics.SetTarget(m_screenTarget.get());

    m_graphics.SetColor(canyon::graphics::BasicColors::Black);
    m_graphics.Clear();

    if (m_root) {
        m_root->Draw();
    }

    // Render target displayed in the top-right corner with a gentle rotation
    if (targetValid) {
        constexpr int kTargetDisplay = 200;
        canyon::IntRect targetRect{
            { GetWidth() - kTargetDisplay - 10, 10 },
            { GetWidth() - 10, kTargetDisplay + 10 }
        };
        float rotation = std::sin(m_elapsedMs * 0.0005f) * 8.0f;
        m_graphics.SetBlendMode(canyon::graphics::BlendMode::Replace);
        m_graphics.SetColor(canyon::graphics::BasicColors::White);
        m_graphics.DrawImage(*m_target->GetImage(), targetRect, nullptr, rotation);
    }

    // Clip rect demo: coloured bands rendered into a constrained region in the
    // bottom-right corner.  The bands are drawn wider than the clip zone so the
    // clipping is clearly visible.
    constexpr int kClipW = 220, kClipH = 80;
    canyon::IntRect clipZone{
        { GetWidth() - kClipW - 10, GetHeight() - kClipH - 10 },
        { GetWidth() - 10, GetHeight() - 10 }
    };
    m_graphics.SetClip(&clipZone);
    m_graphics.SetBlendMode(canyon::graphics::BlendMode::Replace);
    static canyon::graphics::Color const kBands[] = {
        { 0.8f, 0.2f, 0.2f, 1.0f },
        { 0.2f, 0.8f, 0.2f, 1.0f },
        { 0.2f, 0.2f, 0.8f, 1.0f },
        { 0.8f, 0.8f, 0.2f, 1.0f },
    };
    float const bandH = static_cast<float>(kClipH) / 4.0f;
    float const overflowLeft = static_cast<float>(GetWidth() - kClipW - 60);
    float const overflowRight = static_cast<float>(GetWidth() + 60);
    for (int i = 0; i < 4; i++) {
        float top = static_cast<float>(clipZone.topLeft.y) + i * bandH;
        m_graphics.SetColor(kBands[i]);
        m_graphics.DrawFillRectF({ { overflowLeft, top }, { overflowRight, top + bandH } });
    }
    m_graphics.SetClip(nullptr);
    // Outline so the clip zone is easy to spot
    m_graphics.SetColor(canyon::graphics::BasicColors::White);
    m_graphics.DrawRectF({ { static_cast<float>(clipZone.topLeft.x), static_cast<float>(clipZone.topLeft.y) },
                           { static_cast<float>(clipZone.bottomRight.x), static_cast<float>(clipZone.bottomRight.y) } });

    // Screenshot before we return to the swapchain — captures the full scene.
    if (m_pendingScreenshot) {
        m_pendingScreenshot = false;
        m_graphics.DrawToPNG(*m_screenTarget->GetImage(), "screenshot.png");
    }

    // Blit the completed scene to the swapchain.
    m_graphics.SetTarget(nullptr);
    m_graphics.SetBlendMode(canyon::graphics::BlendMode::Replace);
    m_graphics.SetColor(canyon::graphics::BasicColors::White);
    canyon::IntRect fullRect{ { 0, 0 }, { currentSize.x, currentSize.y } };
    m_graphics.DrawImage(*m_screenTarget->GetImage(), fullRect);
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
    }
    if (event.GetClipName() == "transition_out") {
        LoadLayout("assets/layouts/demo.mothui");
        m_root->SetAnimation("transition_in");
        return true;
    }
    return false;
}

bool TestLayer::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down) {
        switch (event.GetKey()) {
        case moth_ui::Key::Escape:
            if (m_layerStack != nullptr) {
                m_layerStack->FireEvent(canyon::EventRequestQuit{});
                return true;
            }
            break;
        case moth_ui::Key::S:
            m_pendingScreenshot = true;
            return true;
        default:
            break;
        }
    }
    return false;
}
