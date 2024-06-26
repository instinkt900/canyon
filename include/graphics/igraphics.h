#pragma once

#include "graphics/iimage.h"
#include "graphics/blend_mode.h"
#include "graphics/color.h"
#include "utils/rect.h"
#include "graphics/text_alignment.h"
#include "graphics/itarget.h"
#include "graphics/ifont.h"

namespace graphics {
    class IGraphics {
    public:
	virtual ~IGraphics() {}
        virtual void SetBlendMode(BlendMode mode) = 0;
        //virtual void SetBlendMode(std::shared_ptr<IImage> target, EBlendMode mode) = 0;
        //virtual void SetColorMod(std::shared_ptr<IImage> target, Color const& color) = 0;
        virtual void SetColor(Color const& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawImage(IImage& image, IntRect const* sourceRect, IntRect const* destRect) = 0;
        virtual void DrawToPNG(std::filesystem::path const& path) = 0;
        virtual void DrawRectF(FloatRect const& rect) = 0;
        virtual void DrawFillRectF(FloatRect const& rect) = 0;
        virtual void DrawLineF(FloatVec2 const& p0, FloatVec2 const& p1) = 0;
        virtual void DrawText(std::string const& text, IFont& font, TextHorizAlignment horizontalAlignment, TextVertAlignment verticalAlignment, IntRect const& destRect) = 0;

        virtual std::unique_ptr<ITarget> CreateTarget(int width, int height) = 0;
        virtual ITarget* GetTarget() = 0;
        virtual void SetTarget(ITarget* target) = 0;

        virtual void SetLogicalSize(IntVec2 const& logicalSize) = 0;
    };
}

