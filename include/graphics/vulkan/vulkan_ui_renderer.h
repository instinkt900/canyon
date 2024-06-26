#pragma once

#include "graphics/color.h"
#include "utils/rect.h"
#include <moth_ui/irenderer.h>

namespace graphics::vulkan {
    class Graphics;
    class UIRenderer : public moth_ui::IRenderer {
    public:
        UIRenderer(Graphics& graphics);
        virtual ~UIRenderer() = default;

        void PushBlendMode(moth_ui::BlendMode mode) override;
        void PopBlendMode() override;
        void PushColor(moth_ui::Color const& color) override;
        void PopColor() override;

        void PushClip(moth_ui::IntRect const& rect) override;
        void PopClip() override;

        void RenderRect(moth_ui::IntRect const& rect) override;
        void RenderFilledRect(moth_ui::IntRect const& rect) override;
        void RenderImage(moth_ui::IImage& image, moth_ui::IntRect const& sourceRect, moth_ui::IntRect const& destRect, moth_ui::ImageScaleType scaleType, float scale) override;
        void RenderText(std::string const& text, moth_ui::IFont& font, moth_ui::TextHorizAlignment horizontalAlignment, moth_ui::TextVertAlignment verticalAlignment, moth_ui::IntRect const& destRect) override;

    private:
        Graphics& m_graphics;
        std::stack<Color> m_drawColor;
        std::stack<BlendMode> m_blendMode;
        std::stack<IntRect> m_clip;
    };
}
