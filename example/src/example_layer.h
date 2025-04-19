#pragma once

#include "canyon/graphics/igraphics.h"
#include <moth_ui/context.h>
#include <moth_ui/events/event_animation.h>
#include <moth_ui/font_factory.h>
#include <moth_ui/layers/layer.h>
#include <moth_ui/utils/vector.h>

class TestLayer : public moth_ui::Layer {
public:
    TestLayer(moth_ui::Context& context, canyon::graphics::IGraphics& graphics, std::filesystem::path const& layoutPath);
    virtual ~TestLayer();

    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw() override;
    void OnAddedToStack(moth_ui::LayerStack* stack) override;
    void LoadLayout(std::filesystem::path const& path);
    bool OnUIEvent(moth_ui::Event const& event);
    bool OnAnimationStopped(moth_ui::EventAnimationStopped const& event);
    // bool OnQuitEvent(QuitEvent  const& event);

private:
    moth_ui::Context& m_context;
    canyon::graphics::IGraphics& m_graphics;
    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::FontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;

    std::shared_ptr<moth_ui::Group> m_root;
    moth_ui::IntVec2 m_lastDrawnSize;
};
