#pragma once

#include "moth_graphics/graphics/asset_context.h"
#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/igraphics.h"
#include <moth_ui/events/event_key.h>
#include <moth_ui/layers/layer.h>

class ExampleLayer : public moth_ui::Layer {
public:
    ExampleLayer(moth_graphics::graphics::IGraphics& graphics, moth_graphics::graphics::AssetContext& assets);

    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw() override;

private:
    bool OnKey(moth_ui::EventKey const& event);

    moth_graphics::graphics::IGraphics& m_graphics;
    std::unique_ptr<moth_graphics::graphics::IFont> m_font;
};
