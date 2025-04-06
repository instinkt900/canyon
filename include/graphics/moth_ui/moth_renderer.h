#pragma once

#include "graphics/igraphics.h"
#include "graphics/color.h"
#include "graphics/blend_mode.h"
#include "utils/rect.h"

#include <moth_ui/irenderer.h>
#include <moth_ui/blend_mode.h>
#include <moth_ui/utils/color.h>
#include <moth_ui/utils/rect.h>

#include <stack>

namespace graphics {
    class MothRenderer : public moth_ui::IRenderer {
    public:
        MothRenderer(IGraphics& graphics);
        virtual ~MothRenderer() = default;

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
        IGraphics& m_graphics;
        std::stack<Color> m_drawColor;
        std::stack<BlendMode> m_blendMode;
        std::stack<IntRect> m_clip;
    };
}
