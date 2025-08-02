#pragma once

#include "canyon/graphics/blend_mode.h"
#include "canyon/graphics/color.h"
#include "canyon/graphics/ifont.h"
#include "canyon/graphics/iimage.h"
#include "canyon/graphics/itarget.h"
#include "canyon/graphics/text_alignment.h"
#include "canyon/utils/rect.h"
#include "canyon/utils/vector.h"

#include <filesystem>
#include <memory>

namespace canyon::platform {
    class Window;
}

namespace canyon::graphics {
    class SurfaceContext;
    class IGraphics {
    public:
        virtual ~IGraphics() {}

        virtual void InitImgui(canyon::platform::Window const& glfwWindow) = 0;

        virtual SurfaceContext& GetContext() const = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void SetBlendMode(BlendMode mode) = 0;
        // virtual void SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) = 0;
        // virtual void SetColorMod(std::shared_ptr<IImage> target, Color const& color) = 0;
        virtual void SetColor(Color const& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawImage(IImage& image, IntRect const& destRect, IntRect const* sourceRect, float rotation) = 0;
        virtual void DrawImageTiled(IImage& image, IntRect const& destRect, IntRect const* sourceRect, float scale) = 0;
        virtual void DrawToPNG(std::filesystem::path const& path) = 0;
        virtual void DrawRectF(FloatRect const& rect) = 0;
        virtual void DrawFillRectF(FloatRect const& rect) = 0;
        virtual void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) = 0;
        virtual void DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) = 0;
        virtual void SetClip(IntRect const* rect) = 0;

        virtual std::unique_ptr<ITarget> CreateTarget(int width, int height) = 0;
        virtual ITarget* GetTarget() = 0;
        virtual void SetTarget(ITarget* target) = 0;

        virtual void SetLogicalSize(IntVec2 const& logicalSize) = 0;
    };
}
