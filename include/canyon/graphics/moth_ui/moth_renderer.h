#pragma once

#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/blend_mode.h"
#include "canyon/utils/rect.h"

#include <moth_ui/graphics/irenderer.h>
#include <moth_ui/graphics/blend_mode.h>
#include <moth_ui/utils/color.h>
#include <moth_ui/utils/rect.h>

#include <stack>

namespace canyon::graphics {
    class MothRenderer : public moth_ui::IRenderer {
    public:
        explicit MothRenderer(IGraphics& graphics);
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

        void SetRendererLogicalSize(moth_ui::IntVec2 const& size) override;

    private:
        IGraphics& m_graphics;
        std::stack<Color> m_drawColor;
        std::stack<BlendMode> m_blendMode;
        std::stack<IntRect> m_clip;
    };
}
