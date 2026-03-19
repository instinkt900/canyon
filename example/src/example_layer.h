#pragma once

#include "canyon/graphics/asset_context.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/igraphics.h"
#include <moth_ui/events/event_key.h>
#include <moth_ui/layers/layer.h>

class ExampleLayer : public moth_ui::Layer {
public:
    ExampleLayer(canyon::graphics::IGraphics& graphics, canyon::graphics::AssetContext& assets);

    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw() override;

private:
    bool OnKey(moth_ui::EventKey const& event);

    canyon::graphics::IGraphics& m_graphics;
    std::unique_ptr<canyon::graphics::IFont> m_font;
};
