#pragma once

#include "graphics/igraphics_context.h"
#include <SDL.h>

namespace graphics::sdl {
    class SDLGraphics : public IGraphicsContext {
    public:
        SDLGraphics(SDL_Renderer* renderer);

        void SetBlendMode(graphics::BlendMode mode) override;
        //void SetBlendMode(std::shared_ptr<graphics::IImage> target, EBlendMode mode) override;
        //void SetColorMod(std::shared_ptr<graphics::IImage> target, graphics::Color const& color) override;
        void SetColor(graphics::Color const& color) override;
        void Clear() override;
        void DrawImage(graphics::IImage& image, IntRect const* sourceRect, IntRect const* destRect) override;
        void DrawToPNG(std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, graphics::IFont& font, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment, IntRect const& destRect) override {}

        std::unique_ptr<graphics::ITarget> CreateTarget(int width, int height) override;
        graphics::ITarget* GetTarget() override;
        void SetTarget(graphics::ITarget* target) override;

        void SetLogicalSize(IntVec2 const& logicalSize) override;

    private:
        SDL_Renderer* m_renderer = nullptr;
    };
}
