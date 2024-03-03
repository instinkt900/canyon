#pragma once

#include "graphics/irenderer.h"
#include <stack>

namespace graphics::vulkan {
    class Graphics;
    class UIRenderer : public IRenderer {
    public:
        UIRenderer(Graphics& graphics);
        virtual ~UIRenderer() = default;

        void PushBlendMode(BlendMode mode) override;
        void PopBlendMode() override;
        void PushColor(Color const& color) override;
        void PopColor() override;

        void PushClip(IntRect const& rect) override;
        void PopClip() override;

        void RenderRect(IntRect const& rect) override;
        void RenderFilledRect(IntRect const& rect) override;
        void RenderImage(IImage& image, IntRect const& sourceRect, IntRect const& destRect, ImageScaleType scaleType, float scale) override;
        void RenderText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) override;

    private:
        Graphics& m_graphics;
        std::stack<Color> m_drawColor;
        std::stack<BlendMode> m_blendMode;
        std::stack<IntRect> m_clip;
    };
}
