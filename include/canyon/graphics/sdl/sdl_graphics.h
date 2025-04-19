#pragma once

#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/igraphics.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/sdl/sdl_surface_context.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/vector.h"

#include <SDL_video.h>
#include <filesystem>
#include <memory>
#include <string>

namespace canyon::graphics::sdl {
    class Graphics : public IGraphics {
    public:
        Graphics(SurfaceContext& context);
        virtual ~Graphics();

        void InitImgui(canyon::platform::Window const& window) override;

        SurfaceContext& GetContext() const override { return m_surfaceContext; }

        void Begin() override;
        void End() override;

        void SetBlendMode(BlendMode mode) override;
        // void SetBlendMode(std::shared_ptr<graphics::IImage> target, graphics::BlendMode mode);
        // void SetColorMod(std::shared_ptr<graphics::IImage> target, graphics::Color const& color) override;
        void SetColor(Color const& color) override;
        void Clear() override;
        void DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect) override;
        void DrawImageTiled(IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) override;
        void DrawToPNG(std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) override;
        void SetClip(IntRect const* rect) override;

        std::unique_ptr<ITarget> CreateTarget(int width, int height) override;
        graphics::ITarget* GetTarget() override;
        void SetTarget(ITarget* target) override;

        void SetLogicalSize(IntVec2 const& logicalSize) override;

    private:
        sdl::SurfaceContext& m_surfaceContext;
        Color m_drawColor;
        BlendMode m_blendMode;
        ITarget* m_currentRenderTarget;
        SDL_Window* m_imguiWindow = nullptr;
    };
}
