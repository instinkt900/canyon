#include "example_layer.h"
#include "moth_graphics/events/event_window.h"
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/layers/layer_stack.h>

ExampleLayer::ExampleLayer(moth_graphics::graphics::IGraphics& graphics, moth_graphics::graphics::AssetContext& assets)
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
        m_graphics.SetColor(moth_graphics::graphics::BasicColors::White);
        moth_graphics::IntRect const rect{ { 0, 0 }, { GetWidth(), GetHeight() } };
        m_graphics.DrawText("Hello, Canyon!", *m_font, rect,
            moth_graphics::graphics::TextHorizAlignment::Center,
            moth_graphics::graphics::TextVertAlignment::Middle);
    }
}

bool ExampleLayer::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down && event.GetKey() == moth_ui::Key::Escape) {
        if (m_layerStack) {
            m_layerStack->FireEvent(moth_graphics::EventRequestQuit{});
            return true;
        }
    }
    return false;
}
