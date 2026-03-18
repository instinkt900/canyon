#include "example_layer.h"
#include "canyon/events/event_window.h"
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/layers/layer_stack.h>

ExampleLayer::ExampleLayer(canyon::graphics::IGraphics& graphics, canyon::graphics::AssetContext& assets)
    : m_graphics(graphics)
    , m_font(assets.FontFromFile("assets/fonts/The-Retropus.ttf", 64)) {
}

bool ExampleLayer::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &ExampleLayer::OnKey);
    return dispatch.GetHandled();
}

void ExampleLayer::Update(uint32_t ticks) {
}

void ExampleLayer::Draw() {
    m_graphics.SetColor({ 0.1f, 0.1f, 0.2f, 1.0f });
    m_graphics.Clear();

    if (m_font) {
        m_graphics.SetColor(canyon::graphics::BasicColors::White);
        canyon::IntRect const rect{ { 0, 0 }, { GetWidth(), GetHeight() } };
        m_graphics.DrawText("Hello, Canyon!", *m_font, rect,
            canyon::graphics::TextHorizAlignment::Center,
            canyon::graphics::TextVertAlignment::Middle);
    }
}

bool ExampleLayer::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down && event.GetKey() == moth_ui::Key::Escape) {
        if (m_layerStack) {
            m_layerStack->FireEvent(canyon::EventRequestQuit{});
            return true;
        }
    }
    return false;
}
