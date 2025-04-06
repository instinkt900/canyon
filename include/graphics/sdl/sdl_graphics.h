#pragma once

#include "graphics/blend_mode.h"
#include "graphics/color.h"
#include "graphics/ifont.h"
#include "graphics/igraphics.h"
#include "graphics/iimage.h"
#include "graphics/itarget.h"
#include "graphics/sdl/sdl_surface_context.h"
#include "graphics/text_alignment.h"
#include "utils/rect.h"
#include "utils/vector.h"

#include <filesystem>
#include <memory>
#include <string>

namespace graphics::sdl {
    class Graphics : public IGraphics {
    public:
        Graphics(SurfaceContext& context);

        SurfaceContext& GetContext() const override { return m_surfaceContext; }

        void Begin() override {}
        void End() override {}

        void SetBlendMode(graphics::BlendMode mode) override;
        // void SetBlendMode(std::shared_ptr<graphics::IImage> target, graphics::BlendMode mode);
        // void SetColorMod(std::shared_ptr<graphics::IImage> target, graphics::Color const& color) override;
        void SetColor(graphics::Color const& color) override;
        void Clear() override;
        void DrawImage(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect) override;
        void DrawImageTiled(graphics::IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) override;
        void DrawToPNG(std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, graphics::IFont& font, graphics::TextHorizAlignment horizontalAlignment, graphics::TextVertAlignment verticalAlignment, IntRect const& destRect) override;
        void SetClip(IntRect const* rect) override;

        std::unique_ptr<graphics::ITarget> CreateTarget(int width, int height) override;
        graphics::ITarget* GetTarget() override;
        void SetTarget(graphics::ITarget* target) override;

        void SetLogicalSize(IntVec2 const& logicalSize) override;

    private:
        graphics::sdl::SurfaceContext& m_surfaceContext;
        graphics::Color m_drawColor;
        graphics::BlendMode m_blendMode;
        graphics::ITarget* m_currentRenderTarget;
    };
}
