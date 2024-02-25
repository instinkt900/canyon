#pragma once

#include "graphics/irenderer.h"
#include <SDL.h>
#include <stack>

namespace graphics::sdl {
    class UIRenderer : public graphics::IRenderer {
    public:
        UIRenderer(SDL_Renderer& renderer);
        virtual ~UIRenderer() = default;

        void PushBlendMode(graphics::BlendMode mode) override;
        void PopBlendMode() override;
        void PushColor(graphics::Color const& color) override;
        void PopColor() override;

        void PushClip(IntRect const& rect) override;
        void PopClip() override;

        void RenderRect(IntRect const& rect) override;
        void RenderFilledRect(IntRect const& rect) override;
        void RenderImage(graphics::IImage& image, IntRect const& sourceRect, IntRect const& destRect, graphics::ImageScaleType scaleType, float scale) override;
        void RenderText(std::string const& text, graphics::IFont& font, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment, IntRect const& destRect) override;

    private:
        SDL_Renderer& m_renderer;
        std::stack<graphics::Color> m_drawColor;
        std::stack<graphics::BlendMode> m_blendMode;
        std::stack<IntRect> m_clip;
    };
}
