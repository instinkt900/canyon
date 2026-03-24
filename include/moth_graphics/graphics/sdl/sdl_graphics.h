#pragma once

#include "moth_graphics/graphics/blend_mode.h"
#include "moth_graphics/graphics/color.h"
#include "moth_graphics/graphics/ifont.h"
#include "moth_graphics/graphics/igraphics.h"
#include "moth_graphics/graphics/iimage.h"
#include "moth_graphics/graphics/itarget.h"
#include "moth_graphics/graphics/sdl/sdl_surface_context.h"
#include "moth_graphics/graphics/text_alignment.h"
#include "moth_graphics/utils/rect.h"
#include "moth_graphics/utils/vector.h"

#include <SDL_video.h>
#include <filesystem>
#include <memory>
#include <stack>
#include <string>

namespace moth_graphics::graphics::sdl {
    class Graphics : public IGraphics {
    public:
        Graphics(SurfaceContext& context);
        ~Graphics() override;

        void InitImgui(moth_graphics::platform::Window const& window) override;

        SurfaceContext& GetSurfaceContext() const override { return m_surfaceContext; }

        void Begin() override;
        void End() override;

        void SetBlendMode(BlendMode mode) override;
        // void SetBlendMode(std::shared_ptr<graphics::IImage> target, graphics::BlendMode mode);
        // void SetColorMod(std::shared_ptr<graphics::IImage> target, graphics::Color const& color) override;
        void SetColor(Color const& color) override;
        void Clear() override;
        void PushTransform(FloatMat4x4 const& transform) override;
        void PopTransform() override;
        void DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect) override;
        void DrawImageTiled(IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) override;
        void DrawToPNG(IImage& image, std::filesystem::path const& path) override;
        void DrawRectF(FloatRect const& rect) override;
        void DrawFillRectF(FloatRect const& rect) override;
        void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) override;
        void DrawText(std::string const& text, IFont& font, IntRect const& destRect, TextHorizAlignment horizontalAlignment = TextHorizAlignment::Left, TextVertAlignment verticalAlignment = TextVertAlignment::Top) override;
        void SetClip(IntRect const* rect) override;

        std::unique_ptr<ITarget> CreateTarget(int width, int height) override;
        graphics::ITarget* GetTarget() override;
        void SetTarget(ITarget* target) override;

        void SetLogicalSize(IntVec2 const& logicalSize) override;

    private:
        FloatMat4x4 CurrentTransform() const;

        sdl::SurfaceContext& m_surfaceContext;
        Color m_drawColor;
        BlendMode m_blendMode = BlendMode::Replace;
        ITarget* m_currentRenderTarget = nullptr;
        SDL_Window* m_imguiWindow = nullptr;
        std::stack<FloatMat4x4> m_transformStack;
    };
}
