#include "example_layer.h"
#include "moth_graphics/events/event_window.h"
#include "moth_graphics/graphics/spritesheet_factory.h"
#include <moth_ui/events/event_dispatch.h>
#include <moth_ui/layers/layer_stack.h>

ExampleLayer::ExampleLayer(moth_graphics::graphics::IGraphics& graphics, moth_graphics::graphics::AssetContext& assets)
    : m_graphics(graphics)
    , m_font(assets.FontFromFile("assets/fonts/The-Retropus.ttf", 64)) {
    auto sheet = assets.GetSpriteSheetFactory().GetSpriteSheet("assets/sprites/character.flipbook.json");

    m_spriteLeft = moth_graphics::graphics::Sprite::Create(sheet);
    m_spriteLeft->SetClip("run_left");
    m_spriteLeft->SetPlaying(true);

    m_spriteRight = moth_graphics::graphics::Sprite::Create(sheet);
    m_spriteRight->SetClip("run_forward");
    m_spriteRight->SetPlaying(true);
}

bool ExampleLayer::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &ExampleLayer::OnKey);
    return dispatch.GetHandled();
}

void ExampleLayer::Update(uint32_t ticks) {
    m_spriteLeft->Update(ticks);
    m_spriteRight->Update(ticks);
}

void ExampleLayer::Draw() {
    m_graphics.SetColor({ 0.1f, 0.1f, 0.2f, 1.0f });
    m_graphics.Clear();

    moth_graphics::IntVec2 spriteLeftPos;
    spriteLeftPos.x = m_spriteLeft->GetWidth() / 2;
    spriteLeftPos.y = GetHeight() / 2;

    moth_graphics::IntVec2 spriteRightPos;
    spriteRightPos.x = GetWidth() - (m_spriteLeft->GetWidth() / 2);
    spriteRightPos.y = GetHeight() / 2;

    if (m_font) {
        m_graphics.SetColor(moth_graphics::graphics::BasicColors::White);
        moth_graphics::IntRect const rect{ { 0, 0 }, { GetWidth(), GetHeight() } };
        std::string_view const message = "Hello, world!";
        m_graphics.DrawText(message, *m_font, rect,
                            moth_graphics::graphics::TextHorizAlignment::Center,
                            moth_graphics::graphics::TextVertAlignment::Middle);

        auto const stringSize = m_font->Measure(message);
        spriteLeftPos.x = (GetWidth() - stringSize.x - m_spriteLeft->GetWidth()) / 2;
        spriteRightPos.x = (GetWidth() + stringSize.x + m_spriteLeft->GetWidth()) / 2;
    }

    m_graphics.DrawSprite(*m_spriteLeft, spriteLeftPos);
    m_graphics.DrawSprite(*m_spriteRight, spriteRightPos);
}

bool ExampleLayer::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down && event.GetKey() == moth_ui::Key::Escape) {
        if (m_layerStack != nullptr) {
            m_layerStack->FireEvent(moth_graphics::EventRequestQuit{});
            return true;
        }
    }
    return false;
}
